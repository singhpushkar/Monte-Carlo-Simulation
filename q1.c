#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<sys/time.h>
#define r 2.0f
struct thread_s{
    int id;
    int iters;
};

void* return_hit(void* arg){
    struct thread_s* data = (struct thread_s*) arg;
    
    long long* no_hits = malloc(sizeof(no_hits));
    *no_hits  = 0;
    float x,y;
    int seed = data->id;
    for(int i=0;i<data->iters;i++){
        x = 1.0*rand_r(&seed)/(RAND_MAX)*2*r - r;
        y = 1.0*rand_r(&seed)/(RAND_MAX)*2*r - r;
        if((x*x + y*y)<(r*r))
            (*no_hits)+=1;
    }
    pthread_exit(no_hits);
}

int main(int args, char** argv){

    int threads[6] = {2,4,8,16,32,40}, loop_count=6;
    FILE* report1=fopen("report1.txt","w"), *data1=fopen("data1.txt","w");
    if(args>=2){
        threads[0] = atoi(argv[1]);
        loop_count=1;
    }
    clock_t start_time, end_time; //starting time

    int i;
    long long N=40000000;
    for(int j =0; j<loop_count;j++){
        long long no_threads = threads[j];
        struct timeval start_val, end_val; 
        gettimeofday(&start_val, NULL);
        pthread_t pids[no_threads];

        for(i=0;i<no_threads;i++){
            struct thread_s data;
            data.id = i;
            data.iters = N/no_threads;
            if(i==0)
                data.iters+= (N%no_threads);
            pthread_attr_t pattr;
            pthread_attr_init(&pattr);
            pthread_create(&pids[i], &pattr, return_hit, &data);

        }
        long long total_hit = 0;
        for(i=0;i<no_threads;i++){
            void *hits;
            pthread_join(pids[i], &hits);
            total_hit += *(long*)hits;
        }

        gettimeofday(&end_val, NULL);
        int x=start_val.tv_sec * (int)1e6 + start_val.tv_usec;
            int y=end_val.tv_sec * (int)1e6 + end_val.tv_usec;
            int time_taken = (y-x); 

        if(j==0)
            fprintf(report1,"%f\n", 4.00*total_hit/N);

        fprintf(report1,"%lf\n",time_taken/1000000.0);
        fprintf(data1,"%lld %lf\n",no_threads, time_taken/1000000.0);
    }
    fclose(report1);
    fclose(data1);

    if(loop_count > 1)
    {
        FILE* plot = popen("gnuplot","w");
        fprintf(plot, "set terminal jpeg size 600,600\n");
        fprintf(plot,"set output \"graph1.jpeg\"\n");
        fprintf(plot,"set xlabel \"No of threads\"\n");
        fprintf(plot,"set ylabel \"Time in seconds\"\n");
        fprintf(plot, "plot \"data1.txt\" linetype 3 linecolor 2 with linespoint\n");
        fclose(plot);
    }
    return 0;
} 