//
// Created by root on 19-7-10.
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include<stdint.h>

#include <unistd.h>
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <sys/types.h>
#include <sys/stat.h>

float total_repetition =1.0;
int is_equal(uint16_t a1[],uint16_t a2[])
{
    int j = 0;
    while(j< sizeof(a1[j]))
    {
        if(a1[j]!=a2[j])
            return 0;
        j++;
    }
    return 1;
}
float compare(uint16_t **hit_location, int cnt)
{
    int ip = 0 , bit = 0, cnt_equal=0;
    for(ip=0;ip<cnt;ip++)
    {
        for(int i=ip+1;i<cnt;i++)
        {
            if(is_equal(hit_location[ip],hit_location[i]))
                cnt_equal++;
        }
    }
    if (cnt <= 1)
        return 1;
    return cnt_equal / ((cnt*(cnt-1)) / 2 );

}



int main(int argc,  char**argv)
{
    mongoc_init ();
    char mongo_url[40]="mongodb://10.0.0.16:27017";
    char *url=NULL, target_name[40], binary_name[40], *target_path=NULL, *ed=NULL;

    uint16_t hit_location[10][65536]={{0}};

    mongoc_cursor_t *cursor;
    mongoc_collection_t *collection;
    bson_iter_t iter;
    bson_t  *doc=bson_new(), *query=bson_new();
    /*while((opt=getopt(argc, argv, "u"))>0)
        switch(opt){
            case 'u':
                url=optarg;
                break;

            default:
                exit(0);
        }

    */
    mongoc_client_t *client=mongoc_client_new (mongo_url);

    bson_error_t error;
    strcpy(target_name,"monitor");
    collection = mongoc_client_get_collection (client, target_name, "repetition");
    //int iter = 0;
    while(1)
    {
        int cnt = 0;
        float repetition = 0.0;
        cursor=mongoc_collection_find_with_opts (collection, query, NULL, NULL);
        bson_subtype_t subtype = BSON_SUBTYPE_BINARY;
        int len = 0;
        while(mongoc_cursor_next (cursor, &doc)){
            bson_iter_init_find(&iter, doc, "trace");
            bson_iter_binary(&iter, &subtype,&len,&hit_location[cnt++]);
        }
        repetition = compare(hit_location,cnt);
        printf("this iter repetition : %3f\n",repetition);
        total_repetition = (total_repetition + repetition) / 2.0;
        sleep(5);
    }

    mongoc_client_destroy (client);
    mongoc_cleanup ();
    return 0;
}




