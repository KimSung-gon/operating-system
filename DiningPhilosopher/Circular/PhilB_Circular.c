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
int signal(int con);

int read_R1();
int read_R2();
int read_R3();

void add_R1(int count);
void add_R2(int count);
void add_R3(int count);

time_t timer;

int L1, L2, L3;
int C1, C2, C3;
int R1, R2, R3;

void main()
{
	key_t semkey = 20103308; 
	
	R1 = R2 = R3 = 0;
	int i;

	if ((L1 = initsem(semkey, 1)) < 0)
		exit(1);
	if ((L2 = initsem(semkey+100, 1)) < 0)
		exit(1);
	if ((L3 = initsem(semkey+200, 1)) < 0)
		exit(1);
	if ((C1 = initsem(semkey+300, 0)) < 0)   
		exit(1);
	if ((C2 = initsem(semkey+400, 0)) < 0)  
		exit(1);
	if ((C3 = initsem(semkey+500, 0)) < 0)  
		exit(1);
		
	for(i=0; i<100; i++)
		Phil_B();
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

int signal(int con) {
	v(con);
}

int read_R1() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("R1.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}

int read_R2() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("R2.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}

int read_R3() {
	int result = 0;
	int time = 0;
	int trash = 0;

	FILE *f;
	f = fopen("R3.txt", "r");
	while (!feof(f))
	{
		fscanf(f, "%d	%d	%d", &result, &time, &trash);
	}
	fclose(f);

	return result;
}


void add_R1(int count) {
	pid = getpid();
	timer = time(NULL);
	FILE *f;
	f = fopen("R1.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}
void add_R2(int count) {
	pid = getpid();
	timer = time(NULL);
	FILE *f;
	f = fopen("R2.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}
void add_R3(int count) {
	pid = getpid();
	timer = time(NULL);
	FILE *f;
	f = fopen("R3.txt", "a");
	fprintf(f, "%d	%d	%d\n", count, pid, timer);
	fclose(f);
	return;
}

Take_R1() {
   p(L1);
   pid = getpid();
   R1 = read_R1();
   while (R1==0) {
        printf("%d wait R1\n", pid);
		wait(C1, L1);
        R1 = read_R1();
        add_R1(R1);
        printf("%d wake up\n", pid);
       }
   R1--;
   add_R1(R1);
   printf("%d take R1\n", pid);
   v(L1);
}

Take_R2() {
   p(L2);
   pid = getpid();
   R2 = read_R2();
   while (R2==0) {
        printf("%d wait R2\n", pid);
		wait(C2, L2);
        R2 = read_R2();
        printf("%d wake up\n", pid);
	   }
   R2--;
   add_R2(R2); 
   printf("%d take R2\n", pid);
   v(L2);
}

Take_R3() {
   p(L3);
   pid = getpid();
   R3 = read_R3();
   while (R3==0) {
        printf("%d wait R3\n", pid);
		wait(C3, L3);
        R3 = read_R3();
        printf("%d wake up\n", pid);
	   }
   R3--;
   add_R3(R3); 
   printf("%d take R3\n", pid);
   v(L3);
}

Put_R1() {
   p(L1);
   pid = getpid();
   R1 = read_R1();
   R1++;
   add_R1(R1);
   signal(C1);
   printf("%d wait R1\n", pid);
   v(L1);
}

Put_R2() {
   p(L2);
   pid = getpid();
   R2 = read_R2();
   R2++;
   add_R2(R2);
   signal(C2);
   printf("%d wait R2\n", pid);
   v(L2);
}

Put_R3() {
   p(L3);
   pid = getpid();
   R3 = read_R3();
   R3++;
   add_R3(R3);
   signal(C3);
   printf("%d wait R3\n", pid);
   v(L3);
}

Phil_A() {
   pid = getpid();
   Take_R1();
   printf("%d is thinking\n", pid);
   sleep(1); 
   printf("%d stop thinking\n", pid);
   Take_R2();
   printf("%d is eating\n", pid);
   sleep(1); 
   printf("%d stop eating\n", pid);
   Put_R1();
   Put_R2();
}

Phil_B() {
   pid = getpid();
   Take_R2();
   printf("%d is thinking\n", pid);
   sleep(1); 
   printf("%d stop thinking\n", pid);
   Take_R3();
   printf("%d is eating\n", pid);
   sleep(1); 
   printf("%d stop eating\n", pid);
   Put_R2();
   Put_R3();
}

Phil_C() {
   pid = getpid();
   Take_R1();
   printf("%d is thinking\n", pid);
   sleep(1); 
   printf("%d stop thinking\n", pid);
   Take_R3();
   printf("%d is eating\n", pid);
   sleep(1); 
   printf("%d stop eating\n", pid);
   Put_R3();
   Put_R1();
}
// Phil_B(), Phil_C()도 구현
// Phil_A, Phil_B, Phil_C 는 별도의 프로세스로 실행되어야 함
