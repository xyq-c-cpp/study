/*
* (C) Radim Kolar 1997-2004
* This is free software, see GNU Public License version 2 for
* details.
*
* Simple forking WWW Server benchmark:
*
* Usage:
*   webbench --help
*
* Return codes:
*    0 - sucess
*    1 - benchmark failed (server is not on-line)
*    2 - bad param
*    3 - internal error, fork failed
* 
*/ 

#include "socket.c"
#include <unistd.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <strings.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
/* values */
volatile int timerexpired=0;
unsigned long speed=0;
unsigned long failed=0;
unsigned long bytes=0;
int exitChild = 0;

/* globals */
int http10=1; /* 0 - http/0.9, 1 - http/1.0, 2 - http/1.1 */
/* Allow: GET, HEAD, OPTIONS, TRACE */
#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_OPTIONS 2
#define METHOD_TRACE 3
#define PROGRAM_VERSION "1.5"
int method=METHOD_GET;
int clients=1;
int allClients = 0;
int force=0;
int force_reload=0;
int proxyport=80;
char *proxyhost=NULL;
int benchtime=30;

bool keep_alive = false;

/* internal */
int mypipe[2];
char host[MAXHOSTNAMELEN];
#define REQUEST_SIZE 2048
char request[REQUEST_SIZE];

static const struct option long_options[]=
{
    {"force",no_argument,&force,1},
    {"reload",no_argument,&force_reload,1},
    {"time",required_argument,NULL,'t'},
    {"help",no_argument,NULL,'?'},
    {"http09",no_argument,NULL,'9'},
    {"http10",no_argument,NULL,'1'},
    {"http11",no_argument,NULL,'2'},
    {"get",no_argument,&method,METHOD_GET},
    {"head",no_argument,&method,METHOD_HEAD},
    {"options",no_argument,&method,METHOD_OPTIONS},
    {"trace",no_argument,&method,METHOD_TRACE},
    {"version",no_argument,NULL,'V'},
    {"proxy",required_argument,NULL,'p'},
    {"clients",required_argument,NULL,'c'},
    {NULL,0,NULL,0}
};

/* prototypes */
static void benchcore(const char* host,const int port, const char *request);
static int bench(void);
static void build_request(const char *url);

static void alarm_handler(int signal)
{
    timerexpired=1;
}	

static void mainAlarmHandler(int signal) {
    printf("\nSpeed=%lu pages/min, %lu bytes/sec.\nRequests: %lu susceed, %lu failed.\n",
        (unsigned long)((speed+failed)/(benchtime/60.0f)),
        (unsigned long)(bytes/(float)benchtime),
        speed,
        failed);
    exit(0);
}

void *ThreadFun(void *arg)
{
    while (1) {
        printf("i am ThreadFun, sleep %d second", benchtime + 5);
        sleep(benchtime + 5);
    }
    return NULL;
}

static void usage(void)
{
    fprintf(stderr,
            "webbench [option]... URL\n"
            "  -f|--force               Don't wait for reply from server.\n"
            "  -r|--reload              Send reload request - Pragma: no-cache.\n"
            "  -t|--time <sec>          Run benchmark for <sec> seconds. Default 30.\n"
            "  -p|--proxy <server:port> Use proxy server for request.\n"
            "  -c|--clients <n>         Run <n> HTTP clients at once. Default one.\n"
            "  -k|--keep                Keep-Alive\n"
            "  -9|--http09              Use HTTP/0.9 style requests.\n"
            "  -1|--http10              Use HTTP/1.0 protocol.\n"
            "  -2|--http11              Use HTTP/1.1 protocol.\n"
            "  --get                    Use GET request method.\n"
            "  --head                   Use HEAD request method.\n"
            "  --options                Use OPTIONS request method.\n"
            "  --trace                  Use TRACE request method.\n"
            "  -?|-h|--help             This information.\n"
            "  -V|--version             Display program version.\n"
           );
}

int main(int argc, char *argv[])
{
    int opt=0;
    int options_index=0;
    char *tmp=NULL;

    if(argc==1)
    {
        usage();
        return 2;
    } 

    while((opt=getopt_long(argc,argv,"912Vfrt:p:c:?hk",long_options,&options_index))!=EOF )
    {
        switch(opt)
        {
            case  0 : break;
            case 'f': force=1;break;
            case 'r': force_reload=1;break; 
            case '9': http10=0;break;
            case '1': http10=1;break;
            case '2': http10=2;break;
            case 'V': printf(PROGRAM_VERSION"\n");exit(0);
            case 't': benchtime=atoi(optarg);break;	
            case 'k': keep_alive = true;break;     
            case 'p': 
            /* proxy server parsing server:port */
            tmp=strrchr(optarg,':');
            proxyhost=optarg;
            if(tmp==NULL)
            {
                break;
            }
            if(tmp==optarg)
            {
                fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
                return 2;
            }
            if(tmp==optarg+strlen(optarg)-1)
            {
                fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
                return 2;
            }
            *tmp='\0';
            proxyport=atoi(tmp+1);break;
            case ':':
            case 'h':
            case '?': usage();return 2;break;
            case 'c': clients=atoi(optarg);break;
        }
    }

    if(optind==argc) {
        fprintf(stderr,"webbench: Missing URL!\n");
        usage();
        return 2;
    }

    if(clients==0) clients=1;
    if(benchtime==0) benchtime=30;
 
    /* Copyright */
    fprintf(stderr,"Webbench - Simple Web Benchmark "PROGRAM_VERSION"\n"
            "Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.\n"
            );
 
    build_request(argv[optind]);
 
    // print request info ,do it in function build_request
    /*printf("Benchmarking: ");
 
    switch(method)
    {
        case METHOD_GET:
        default:
        printf("GET");break;
        case METHOD_OPTIONS:
        printf("OPTIONS");break;
        case METHOD_HEAD:
        printf("HEAD");break;
        case METHOD_TRACE:
        printf("TRACE");break;
    }
    
    printf(" %s",argv[optind]);
    
    switch(http10)
    {
        case 0: printf(" (using HTTP/0.9)");break;
        case 2: printf(" (using HTTP/1.1)");break;
    }
 
    printf("\n");
    */

    printf("Runing info: ");

    if(clients==1) 
        printf("1 client");
    else
        printf("%d clients",clients);

    printf(", running %d sec", benchtime);
    
    if(force) printf(", early socket close");
    if(proxyhost!=NULL) printf(", via proxy server %s:%d",proxyhost,proxyport);
    if(force_reload) printf(", forcing reload");
    
    printf(".\n");
    
    return bench();
}

void build_request(const char *url)
{
    char tmp[10];
    int i;

    //bzero(host,MAXHOSTNAMELEN);
    //bzero(request,REQUEST_SIZE);
    memset(host,0,MAXHOSTNAMELEN);
    memset(request,0,REQUEST_SIZE);

    if(force_reload && proxyhost!=NULL && http10<1) http10=1;
    if(method==METHOD_HEAD && http10<1) http10=1;
    if(method==METHOD_OPTIONS && http10<2) http10=2;
    if(method==METHOD_TRACE && http10<2) http10=2;

    switch(method)
    {
        default:
        case METHOD_GET: strcpy(request,"GET");break;
        case METHOD_HEAD: strcpy(request,"HEAD");break;
        case METHOD_OPTIONS: strcpy(request,"OPTIONS");break;
        case METHOD_TRACE: strcpy(request,"TRACE");break;
    }

    strcat(request," ");

    if(NULL==strstr(url,"://"))
    {
        fprintf(stderr, "\n%s: is not a valid URL.\n",url);
        exit(2);
    }
    if(strlen(url)>1500)
    {
        fprintf(stderr,"URL is too long.\n");
        exit(2);
    }
    if (0!=strncasecmp("http://",url,7)) 
    { 
        fprintf(stderr,"\nOnly HTTP protocol is directly supported, set --proxy for others.\n");
        exit(2);
    }
    
    /* protocol/host delimiter */
    i=strstr(url,"://")-url+3;

    if(strchr(url+i,'/')==NULL) {
        fprintf(stderr,"\nInvalid URL syntax - hostname don't ends with '/'.\n");
        exit(2);
    }
    
    if(proxyhost==NULL)
    {
        /* get port from hostname */
        if(index(url+i,':')!=NULL && index(url+i,':')<index(url+i,'/'))
        {
            strncpy(host,url+i,strchr(url+i,':')-url-i);
            //bzero(tmp,10);
            memset(tmp,0,10);
            strncpy(tmp,index(url+i,':')+1,strchr(url+i,'/')-index(url+i,':')-1);
            /* printf("tmp=%s\n",tmp); */
            proxyport=atoi(tmp);
            if(proxyport==0) proxyport=80;
        } 
        else
        {
            strncpy(host,url+i,strcspn(url+i,"/"));
        }
        // printf("Host=%s\n",host);
        strcat(request+strlen(request),url+i+strcspn(url+i,"/"));
    } 
    else
    {
        // printf("ProxyHost=%s\nProxyPort=%d\n",proxyhost,proxyport);
        strcat(request,url);
    }

    if(http10==1)
        strcat(request," HTTP/1.0");
    else if (http10==2)
        strcat(request," HTTP/1.1");
  
    strcat(request,"\r\n");
  
    if(http10>0)
        strcat(request,"User-Agent: WebBench "PROGRAM_VERSION"\r\n");
    if(proxyhost==NULL && http10>0)
    {
        strcat(request,"Host: ");
        strcat(request,host);
        strcat(request,"\r\n");
    }
 
    if(force_reload && proxyhost!=NULL)
    {
        strcat(request,"Pragma: no-cache\r\n");
    }
  
    if(http10>1)
    {
        if (!keep_alive)
            strcat(request,"Connection: close\r\n");
        else
            strcat(request,"Connection: Keep-Alive\r\n");
    }
        
    
    /* add empty line at end */
    if(http10>0) strcat(request,"\r\n"); 
    
    printf("\nRequest:\n%s\n",request);
}

/* vraci system rc error kod */
static int bench(void)
{
    int i,j,k;	
    pid_t pid=0;
    FILE *f;

    /* check avaibility of target server */
    i=Socket(proxyhost==NULL?host:proxyhost,proxyport);
    if(i<0) { 
        fprintf(stderr,"\nConnect to server failed. Aborting benchmark.\n");
        return 1;
    }
    close(i);
    
    /* create pipe */
    if(pipe(mypipe))
    {
        perror("pipe failed.");
        return 3;
    }

    /* not needed, since we have alarm() in childrens */
    /* wait 4 next system clock tick */
    /*
    cas=time(NULL);
    while(time(NULL)==cas)
    sched_yield();
    */

    /* fork childs */
    for(i=0;i<clients;i++)
    {
        pid=fork();
        if(pid <= (pid_t) 0)
        {
                // if (errno == 11 && pid != 0)
                // {
                //     printf("here i = %d\n", i);
                //     while (fork() < 0);
                //     continue;
                // }
            /* child process or error*/
            sleep(1); /* make childs faster */
            break;
        }
    }

    if( pid < (pid_t) 0)
    {
        fprintf(stderr,"problems forking worker no. %d\n",i);
        perror("fork failed.");
        return 3;
    }

    if(pid == (pid_t) 0)
    {
        /* I am a child */
        if(proxyhost==NULL)
            benchcore(host,proxyport,request);
        else
            benchcore(proxyhost,proxyport,request);

        /* write results to pipe */
        f=fdopen(mypipe[1],"w");
        if(f==NULL)
        {
            perror("open pipe for writing failed.");
            return 3;
        }
        /* fprintf(stderr,"Child - %d %d\n",speed,failed); */
        fprintf(f,"%d %d %d\n-1 -1 -1\n",speed,failed,bytes);
        sleep(1);
        fclose(f);

        return 0;
    } 
    else
    {
        /* pthread_t myThread1;
        // 创建 myThread1 线程
        int res = pthread_create(&myThread1, NULL, ThreadFun, NULL);
        if (res != 0) {
            printf("线程创建失败");
            return 0;
        }
        sleep(1); */

        struct sigaction sa;

        /* setup alarm signal handler */
        sa.sa_handler=mainAlarmHandler;
        sa.sa_flags=0;
        if(sigaction(SIGALRM,&sa,NULL))
            exit(3);
        
        alarm(benchtime + 5);

        f=fdopen(mypipe[0],"r");
        if(f==NULL) 
        {
            perror("open pipe for reading failed.");
            return 3;
        }
        
        setvbuf(f,NULL,_IONBF,0);
        
        speed=0;
        failed=0;
        bytes=0;
    
        while(1)
        {
            pid=fscanf(f,"%d %d %d",&i,&j,&k);
            if(pid<2)
            {
                fprintf(stderr,"Some of our childrens died.\n");
                break;
            }
            if (i == -1 && j == -1 && k == -1) {
                --clients;
                if (clients == 0)
                    break;
                continue;
            }

            speed+=i;
            failed+=j;
            bytes+=k;
        
            /* fprintf(stderr,"*Knock* %d %d read=%d\n",speed,failed,pid); */
            //if(==0) break;
        }
    
        fclose(f);

        printf("\nSpeed=%lu pages/min, %lu bytes/sec.\nRequests: %lu susceed, %lu failed.\n",
            (unsigned long)((speed+failed)/(benchtime/60.0f)),
            (unsigned long)(bytes/(float)benchtime),
            speed,
            failed);
    }
    
    return i;
}

void benchcore(const char *host,const int port,const char *req)
{
    int rlen;
    char buf[1500];
    int s,i;
    struct sigaction sa;

    /* setup alarm signal handler */
    sa.sa_handler=alarm_handler;
    sa.sa_flags=0;
    if(sigaction(SIGALRM,&sa,NULL))
        exit(3);
    
    alarm(benchtime); // after benchtime,then exit

    rlen=strlen(req);

    //printf("keep_alive\n");


    if (keep_alive)
    {
        while ((s = Socket(host,port)) == -1);  
        nexttry1:while(1)
        {
            if(timerexpired)
            {
                if(failed>0)
                {
                    /* fprintf(stderr,"Correcting failed by signal\n"); */
                    failed--;
                }
                return;
            }
           
            if(s<0) { failed++;continue;} 
            if(rlen!=write(s,req,rlen)) {
                failed++;
                close(s);
                while ((s = Socket(host,port)) == -1);
                continue;
            }
            if(force==0) 
            {
                /* read all available data from socket */
                while(1)
                {
                    if(timerexpired) break; 
                    i=read(s,buf,1500);
                    /* fprintf(stderr,"%d\n",i); */
                    if(i<0) 
                    { 
                        failed++;
                        close(s);
                        //while ((s = Socket(host,port)) == -1);
                        goto nexttry1;
                    }
                    else
                    if(i==0) break;
                    else
                    bytes+=i;
                    // Supposed reveived bytes were less than 1500
                    //if (i < 1500) 
                        break;
                }
            }
            speed++;
        }
    }
    else 
    {
        nexttry:while(1)
        {
            if(timerexpired)
            {
                if(failed>0)
                {
                    /* fprintf(stderr,"Correcting failed by signal\n"); */
                    failed--;
                }
                return;
            }
            s=Socket(host,port);
            if(s<0) { failed++;continue;} 
            if(rlen!=write(s,req,rlen)) {failed++;close(s);continue;}
            if(http10==0) 
            if(shutdown(s,1)) { failed++;close(s);continue;}
            if(force==0) 
            {
                /* read all available data from socket */
                while(1)
                {
                    if(timerexpired) break; 
                    i=read(s,buf,1500);
                    if(i<0) 
                    { 
                        failed++;
                        close(s);
                        goto nexttry;
                    }
                    else
                    if(i==0) break;
                    else
                    bytes+=i;
                }
            }
            if(close(s)) {failed++; continue;}
            speed++;
        }
    }
        
    
}