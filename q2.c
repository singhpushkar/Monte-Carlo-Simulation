#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<math.h>
#include<sys/time.h>

#define SIZE 20
int** global_array;
int M;
double t_value;

struct thread_s{
    int id;
    int iters;
};
void readfile(){
    FILE* fptr;
    int a,b,i;
    if ((fptr = fopen("data.dat", "r")) == NULL)
    {
        printf("Error! opening file");
        exit(1);         
    }
    M=0;
    while(fscanf(fptr, "%d%d",&a,&b) ==2)
        M++;
    global_array = malloc(M*sizeof(int*));
    fclose(fptr);
    fptr = fopen("data.dat", "r");
    for(i=0;i<M;i++){
        global_array[i] = malloc(2*sizeof(int));
        fscanf(fptr, "%d%d",&global_array[i][0],&global_array[i][1]);
    }
    fclose(fptr);
}

double t_val()
{
    double x1 = 0.0, x2 = 0.0, s1 = 0.0,s2 = 0.0;
    for(int i=0;i<M;i++){
        x1 = x1 + global_array[i][0];
        x2 = x2 + global_array[i][1];
    }
    x1 = x1/M;
    x2 = x2/M;
    for(int i=0;i<M;i++){
        s1 = s1 + pow((global_array[i][0] - x1),2);
        s2 = s2 + pow((global_array[i][1] - x2),2);
    }
    s1/=(M*M);
    s2/=(M*M);
    t_value = (x2 - x1)/sqrt((s1+s2));
}

void* compute_t(void* arg){
    struct thread_s* data = (struct thread_s*) arg;
     long long* tval = malloc(sizeof(tval));
     *tval = 0;
    int seed = data->id;
    for(int j=0;j<data->iters;j++)
    {
        int array[SIZE][2],i;
        for(i=0;i<SIZE;i++){
            int rnd = rand_r(&seed)%M;
            array[i][0] = global_array[rnd][0];
            array[i][1] = global_array[rnd][1];
        }
            
        double x1 = 0.0, x2 = 0.0, s1 = 0.0,s2 = 0.0;
        for(i=0;i<SIZE;i++){
            x1 = x1 + array[i][0];
            x2 = x2 + array[i][1];
        }
        x1 = x1/SIZE;
        x2 = x2/SIZE;
        for(i=0;i<SIZE;i++){
            s1 = s1 + pow((array[i][0] - x1),2);
            s2 = s2 + pow((array[i][1] - x2),2);
        }
        s1/=(SIZE*SIZE);
        s2/=(SIZE*SIZE);
        if((x2 - x1)/sqrt((s1+s2)) >= t_value){
            (*tval)++;
        }
    }
    pthread_exit(tval);
}

int main(int args, char** argv){
    int threads[6] = {2,4,8,16,32,40}, loop_count=6;
    FILE* report2=fopen("report2.txt","w"), *data2=fopen("data2.txt","w");
    if(args>=2){
        threads[0] = atoi(argv[1]);
        loop_count=1;
    }
    int i;
    readfile();
    t_val();
    long long N= 1000000;
    for(int j =0; j<loop_count;j++){
        // time_interval start_time, end_time;
        long long no_threads = threads[j];
        // printf("%lf\n",t_value);
        // start_time = clock();
        struct timeval start_val, end_val; 
        gettimeofday(&start_val, NULL);
        readfile();
        pthread_t pids[no_threads];
        for(i=0;i<no_threads;i++){
            struct thread_s data;
            data.id = i;
            data.iters = N/no_threads;
            if(i==0)
                data.iters+= (N%no_threads);

            pthread_attr_t pattr;
            pthread_attr_init(&pattr);
            pthread_create(&pids[i], &pattr, compute_t, &data);
        }
        long long total_t = 0;
        for(i=0;i<no_threads;i++){
            void *t;
            pthread_join(pids[i], &t);
            total_t += *(long*)t;
        }
        // end_time = clock();
        gettimeofday(&end_val, NULL);
        if(j==0)
            fprintf(report2,"%lld\n", total_t);

            int x=start_val.tv_sec * (int)1e6 + start_val.tv_usec;
            int y=end_val.tv_sec * (int)1e6 + end_val.tv_usec;
            int time_taken = (y-x); 
        // fprintf(report2,"%lf\n",((double) (end_time - start_time))*1000/CLOCKS_PER_SEC);
        fprintf(report2,"%lf\n",((double) (time_taken))/1000000.0);
        fprintf(data2,"%lld %lf\n",no_threads, ((double) (time_taken))/1000000.0);;
    }
    fclose(report2);
    fclose(data2);

    if(loop_count > 1)
    {
        FILE* plot = popen("gnuplot","w");
        fprintf(plot, "set terminal jpeg size 600,600\n");
        fprintf(plot,"set output \"graph2.jpeg\"\n");
        fprintf(plot,"set xlabel \"No of threads\"\n");
        fprintf(plot,"set ylabel \"Time in mili seconds\"\n");
        fprintf(plot, "plot \"data2.txt\" linetype 3 linecolor 2 with linespoint\n");
        fclose(plot);
    }
    return 0;
}