#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include<stdint.h>
#include<unistd.h>
#include"hash.h"
#define MAP_SIZE  (1 << 16)
#define HASH_CONST          0xa5b35705
// #define MAP_SIZE  3
//$(pkg-config --libs --cflags libmongoc-1.0)    add this command to gcc option to load mongodb driver

 
/* Get unix time in milliseconds */

static u64 get_cur_time(void) {

  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);

  return (tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000);

}

void send_binary(mongoc_client_t *client, const char* target_name, char* target_path, char *binary_name, char *describe)
{
	mongoc_collection_t *collection;
	bson_error_t error;
	bson_t *doc=bson_new();
	char name[10];

  	collection = mongoc_client_get_collection (client, target_name, "binary");
  	FILE *fp=fopen(target_path, "rb");
	if(!fp)
		printf("open file error\n");
	fseek(fp, 0L, SEEK_END);
	int size=ftell(fp);
	fseek(fp, 0L, SEEK_SET);
 	uint8_t *buf=malloc(size);
 	fread(buf,  sizeof(uint8_t), size, fp);
	fclose(fp);

	int num=size/15000000+1;
	if(num==1)
	{
		BSON_APPEND_BINARY (doc, "code", BSON_SUBTYPE_BINARY, buf, size);
		BSON_APPEND_UTF8(doc, "name", binary_name);
		BSON_APPEND_INT32(doc, "length", size);
		BSON_APPEND_INT32(doc, "flag", 1);
		if(describe!=NULL)
			BSON_APPEND_UTF8(doc, "description", describe);
		if (!mongoc_collection_insert_one (
		collection, doc, NULL, NULL, &error)) {
		fprintf (stderr, "%s\n", error.message);
		}
	}
	else
	{
		for(int i=0; i<num; i++)
		{
			sprintf(name, "code%d", i);
			if(i!=num-1)
				BSON_APPEND_BINARY (doc, "code", BSON_SUBTYPE_BINARY, buf+i*15000000, 15000000);
			else
				BSON_APPEND_BINARY (doc, "code", BSON_SUBTYPE_BINARY, buf+i*15000000, size%(i*15000000));
			if(i==0)
			{
				BSON_APPEND_UTF8(doc, "name", binary_name);
				BSON_APPEND_INT32(doc, "length", size);
				BSON_APPEND_INT32(doc, "flag", 1);
			}
			else
				BSON_APPEND_INT32(doc, "flag", 0);
			if (!mongoc_collection_insert_one (
			collection, doc, NULL, NULL, &error)) {
			fprintf (stderr, "%s\n", error.message);
			}
			bson_destroy(doc);
			doc=bson_new();
		}
	}
	
  	bson_destroy(doc);

	mongoc_collection_destroy (collection);
	
	return;
}




void send_seed(mongoc_client_t *client, const char* target_name, u8 *in_dir)
{
	mongoc_collection_t *collection1, *collection2;
	bson_error_t error;
	bson_t *doc;
	uint32_t seed_key;
	int size;

  	collection1 = mongoc_client_get_collection (client, target_name, "SeedBook");
  	collection2 = mongoc_client_get_collection (client, target_name, "seed");
  	if(in_dir==NULL)
	  	in_dir="input";
  	u8 locate[100];
  	struct dirent **nl;
  	int nl_cnt;
  	FILE *fp;
  	long ii;
  	nl_cnt=scandir(in_dir, &nl, NULL, alphasort);
  	if(nl_cnt<0){
  		printf("indir connot open\n");
  		return;
  	}
  	for(int i=0; i<nl_cnt; i++){
  		if(nl[i]->d_name[0]=='.')
  			continue;
  		ii=get_cur_time();
  		sprintf(locate,"%s/%s",in_dir, nl[i]->d_name);
  		fp=fopen(locate, "r");
  		if(!fp)
  			printf("open file error\n");
	  	fseek(fp, 0L, SEEK_END);
	  	size=ftell(fp);
	  	fseek(fp, 0L, SEEK_SET);
	    uint8_t *buf=malloc(size);
	    fread(buf,  sizeof(uint8_t), size, fp);
	  	fclose(fp);
	  	seed_key=abs(hash32(buf,size,HASH_CONST));

	  	doc=bson_new();
	  	BSON_APPEND_INT32(doc,"len", size);
	  	sprintf(locate, "init%d", seed_key);
	  	BSON_APPEND_UTF8(doc,"fname", locate);
	  	BSON_APPEND_INT64(doc, "time", ii);
	  	if (!mongoc_collection_insert_one (
         collection1, doc, NULL, NULL, &error)) {
      		fprintf (stderr, "%s\n", error.message);
  		}
  		bson_destroy(doc);
  		doc=bson_new();
  		BSON_APPEND_UTF8 (doc, "fname", locate);
		BSON_APPEND_BINARY(doc, "seed", BSON_SUBTYPE_BINARY, buf, size);
		BSON_APPEND_INT32(doc, "len", size);
		BSON_APPEND_UTF8(doc,"describe", "original_seeds");
  		BSON_APPEND_INT64(doc,"time",ii);
	  	if (!mongoc_collection_insert_one (
         collection2, doc, NULL, NULL, &error)) {
      		fprintf (stderr, "%s\n", error.message);
  		}
  		bson_destroy(doc);

		free(buf);
  	}
  	mongoc_collection_destroy (collection1);
  	mongoc_collection_destroy (collection2);
	
	return;
}

void usage()
{
	printf("Required parameters:\n\n-u       - the url of mongodb server\n");
    printf("-b       - the excuting binary name or the database name\n\n");
}

int main(int argc,  char**argv)
{
	mongoc_init ();
	char mongo_url[40]="mongodb://";
	char *url=NULL, target_name[40], binary_name[40], *target_path=NULL, *ed=NULL;
	char *describe=NULL;
	u8 *in_dir=NULL;
	s32 opt;
	while((opt=getopt(argc, argv, "u:b:i:e:m:"))>0)
		switch(opt){
			case 'u':
				url=optarg;
				break;
			case 'b':
				target_path=optarg;
				break;
			case 'i':
				in_dir=optarg;
				break;
			case 'e':
				ed=optarg;
				break;
			case 'm':
				describe=optarg;
				break;
			default:
				usage();
		}
	if (!url || !target_path)
		{
			usage();
			return 0;
		}
 	else
 		strcat(mongo_url, url);
 	if(!url)
 		strcpy(mongo_url, "mongodb://localhost:27017");
 	if(!target_path)
 		target_path="yinqidi";
 	if(strrchr(target_path, '/'))
 		strcpy(target_name, strrchr(target_path, '/')+1);
 	else
 		strcpy(target_name, target_path);
 	strcpy(binary_name, target_name);
 	if(ed!=NULL)
 		strcat(target_name, ed);
 	mongoc_client_t *client=mongoc_client_new (mongo_url);
 	int ii;
 	char ** names;
 	bson_error_t error;
 	if((names=mongoc_client_get_database_names_with_opts (client, NULL, &error)))
 	{
 		for (ii = 0; names[ii]; ii++)
 		{
        	if(strcmp(names[ii], target_name)==0){
        		printf("the target already exists\n");
        		bson_strfreev (names);
        		mongoc_client_destroy (client);
				mongoc_cleanup ();
        		return 0;
        	}
        }
      	bson_strfreev (names);
   	} 
   	else {
      fprintf (stderr, "Command failed: %s\n", error.message);
   	}

	send_binary(client, target_name, target_path, binary_name, describe);
	if(!in_dir)
		send_seed(client, target_name, NULL);
	else
		send_seed(client, target_name, in_dir);
	
	mongoc_client_destroy (client);
	mongoc_cleanup ();
	return 0;
}
