#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define SEMPERM 0600
#define TRUE 1
#define FALSE 0
typedef union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
}semun;

int length = 0;
pid_t pid;

int initsem(key_t semkey, int n);
int p(int semid);
int v(int semid);

int wait(int con, int lock);
int signal(int queue, int con);

int read_AW();
int read_AR();
int read_WW();
int read_WR();

void add_AW(int count);
void add_AR(int count);
void add_WW(int count);
void add_WR(int count);

void check_start();	

time_t timer;



void main(int argc, char *argv[])
{
	key_t semkey = 20103308; 
	key_t writer = semkey + 100;	//
	key_t reader = semkey + 200;	//
	int lock;
	int con;
	int conR;
	int i = 0, j = 0;
	check_start();

	sleep(atoi(argv[1]));
	int AR = 0, AW = 0, WW = 0, WR = 0;

	if ((lock = initsem(semkey, 1)) < 0)
		exit(1);
	if ((con = initsem(writer, 0)) < 0)   
		exit(1);
	if ((conR = initsem(reader, 0)) < 0)  
		exit(1);
	
	p(lock);
	AW = read_AW();	
	AR = read_AR();
	WW = read_WW();
	WR = read_WR();
	
	while ((AW + WW) > 0) {	
		if(i == 0) {	//
			WR ++;
			add_WR(WR);	
			WR --;
			i = 1;		//
		}
		wait(conR, lock);	
		AW = read_AW();	
		WW = read_WW();	
	}

	AR = read_AR();	
	AR++;			
	add_AR(AR);
	v(lock);

	sleep(atoi(argv[2]));	
	p(lock);
	AR = read_AR();	
	AR--;
	add_AR(AR);	

	WW = read_WW();	
	AR = read_AR();	
	if (AR == 0 && WW > 0) {
		signal(WW, con);	 
		WW--;		
		add_WW(WW);		
	}
	
	v(lock);
	exit(0);
}

int initsem(key_t semkey, int n)
{
	int status = 0, semid;
	if ((semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL)) == -1)
	{
		if (errno == EEXIST)
			semid = semget(semkey, 1, 0);
	}
	else
	{
		semun arg;
		arg.val = n;
		status = semctl(semid, 0, SETVAL, arg);
	}
	if (semid == -1 || status == -1)
	{
		perror("initsem failed");
		return (-1);
	}
	return (semid);
}

int p(int semid)
{
	struct sembuf p_buf;
	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;
	if (semop(semid, &p_buf, 1) == -1)
	{
		perror("p(semid) failed");
		exit(1);
	}
	return (0);
}

int v(int semid)
{
	struct sembuf v_buf;
	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;
	if (semop(semid, &v_buf, 1) == -1)
	{
		perror("v(semid) failed");
		exit(1);
	}
	return (0);
}

int wait(int con, int lock) {
	v(lock);
	p(con);
	p(lock);
	return (0);

}

int signal(int queue, int con) {
	if (queue > 0)
		v(con);
}

int read_AW() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("AW.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}

int read_AR() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("AR.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}

int read_WW() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("WW.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}

int read_WR() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("WR.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}


void add_AW(int count) {	
	pid = getpid();timer = time(NULL);
	FILE *f;
	f = fopen("AW.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}

void add_AR(int count) {
	pid = getpid();timer = time(NULL);
	FILE *f;
	f = fopen("AR.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}
void add_WW(int count) {
	pid = getpid();
	timer = time(NULL);
	FILE *f;
	f = fopen("WW.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}
void add_WR(int count) {
	pid = getpid();
	timer = time(NULL);
	FILE *f;
	f = fopen("WR.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}

void check_start() {
	int result = 0;
	int times = 0;
	int trash = 0;
	FILE *f;
	timer = time(NULL);
	f = fopen("AR.txt", "r");
	while (!feof(f))
	{
		timer = time(NULL);
		fscanf(f, "%d	%d	%d", &result, &times, &trash);
	}
	fclose(f);

	if (trash == 0) {
		f = fopen("AR.txt", "w");
		fprintf(f, "%d	%d	%d\n", 0, 0, timer);
		fclose(f);
	}
	return;
	f = fopen("WR.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &times, &trash);
	}
	fclose(f);

	if (trash == 0) {
		f = fopen("WR.txt", "w");
		fprintf(f, "%d	%d	%d\n", 0, 0, timer);
		fclose(f);
	}
	return;
}
