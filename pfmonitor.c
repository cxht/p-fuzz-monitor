//
// Created by root on 19-7-10.
//
#include <unistd.h>
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#define GROUP_SIZE 100;
int main(int argc,  char**argv)
{
    mongoc_init ();
    char mongo_url[40]="mongodb://";
    char *url=NULL, target_name[40], binary_name[40], *target_path=NULL, *ed=NULL;
    char *describe=NULL;
    u16 hit_location[MAP_SIZE][MAP_SIZE];
    u8 *in_dir=NULL;
    s32 opt;
    mongoc_cursor_t *cursor;
    bson_iter_t iter;
    bson_t  *doc=bson_new(), *query=bson_new();
    while((opt=getopt(argc, argv, "u"))>0)
        switch(opt){
            case 'u':
                url=optarg;
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

    mongoc_client_t *client=mongoc_client_new (mongo_url);

    bson_error_t error;
    target_name = "monitor"
    collection = mongoc_client_get_collection (client, target_name, "repetition");
    int iter = 0;
    while(1)
    {
        int cnt = 0;
        cursor=mongoc_collection_find_with_opts (collection, query, opts, NULL);
        while(mongoc_cursor_next (cursor, &doc)){
            bson_iter_init_find(&iter, doc, "trace");
            hit_location[cnt++]=bson_iter_binary(&iter, NULL);
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

void compare(u16 **hit_location, int cnt)
{
    int ip = 0 , bit = 0, cnt_equal=0;
    for(ip=0;ip<cnt;ip++)
    {
        for(i=ip+1;i<cnt;i++)
        {
            if(is_equal(hit_location[ip],hit_location[i]))
                cnt_equal++;
        }
    }
    return cnt_equal / ((cnt*(cnt-1)) / 2 )

}
int is_equal(a1[],a2[])
{
    int j = 0;
    while(j< sizeof(a1[i]))
    {
        if(a1[j]!=a2[j])
            return 0;
        j++;
    }
    return 1;
}



