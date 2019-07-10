//
// Created by root on 19-7-10.
//

int main(int argc,  char**argv)
{
    mongoc_init ();
    char mongo_url[40]="mongodb://";
    char *url=NULL, target_name[40], binary_name[40], *target_path=NULL, *ed=NULL;
    char *describe=NULL;
    u8 *in_dir=NULL;
    s32 opt;
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

    //build_monitor(client, target_name);

    mongoc_client_destroy (client);
    mongoc_cleanup ();
    return 0;
}




