// Copyright (c) 2020 Trevor Bakker
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
  
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/*** Constants that define parameters of the simulation ***/

#define MAX_SEATS 3        /* Number of seats in the professor's office */
#define professor_LIMIT 10 /* Number of students the professor can help before he needs a break */
#define MAX_STUDENTS 1000  /* Maximum number of students in the simulation */



#define CLASSA 0
#define CLASSB 1
#define CLASSC 2
#define CLASSD 3
#define CLASSE 4

/* TODO */
/* Add your synchronization variables here */

pthread_mutex_t lock;
pthread_mutex_t prof_lock;

pthread_cond_t load_in;
pthread_cond_t hold;
pthread_cond_t conditiona;
pthread_cond_t conditionb;
pthread_cond_t arrivea=PTHREAD_COND_INITIALIZER;
pthread_cond_t arriveb=PTHREAD_COND_INITIALIZER;


sem_t class_seatA;
sem_t class_seatB;
sem_t class_seat;

/* Basic information about simulation.  They are printed/checked at the end 
 * and in assert statements during execution.
 *
 * You are responsible for maintaining the integrity of these variables in the 
 * code that you develop. 
 */

static int students_in_office;   /* Total numbers of students currently in the office */
static int classa_inoffice;      /* Total numbers of students from class A currently in the office */
static int classb_inoffice;      /* Total numbers of students from class B in the office */
static int students_since_break = 0;
static int classa_consec;
static int classb_consec;
static int empty_flag;
static int first_flag;
static int classa_waiting;
static int classb_waiting;
static int test;


typedef struct 
{
  int arrival_time;  // time between the arrival of this student and the previous student
  int question_time; // time the student needs to spend with the professor
  int student_id;
  int class;
} student_info;

/* Called at beginning of simulation.  
 * TODO: Create/initialize all synchronization
 * variables and other global variables that you add.
 */
static int initialize(student_info *si, char *filename) 
{
  students_in_office = 0;
  classa_inoffice = 0;
  classb_inoffice = 0;
  students_since_break = 0;
  classa_consec = 0;
  classb_consec = 0;
  empty_flag=0;
  first_flag=0;
  classa_waiting=0;
  classb_waiting=0;
  test=0;


  pthread_cond_init(&hold,NULL);

  sem_init(&class_seat,0,MAX_SEATS);
  sem_init(&class_seatA,0,3);
  sem_init(&class_seatB,0,3);

  /* Initialize your synchronization variables (and 
   * other variables you might use) here
   */


  /* Read in the data file and initialize the student array */
  FILE *fp;

  if((fp=fopen(filename, "r")) == NULL) 
  {
    printf("Cannot open input file %s for reading.\n", filename);
    exit(1);
  }

  int i = 0;
  while ( (fscanf(fp, "%d%d%d\n", &(si[i].class), &(si[i].arrival_time), &(si[i].question_time))!=EOF) && 
           i < MAX_STUDENTS ) 
  {
    i++;
  }

 fclose(fp);
 return i;
}

/* Code executed by professor to simulate taking a break 
 * You do not need to add anything here.  
 */
static void take_break()
{
  printf("The professor is taking a break now.\n");
  sleep(5);
  assert( students_in_office == 0 );
  students_since_break = 0;
}

/* Code for the professor thread. This is fully implemented except for synchronization
 * with the students.  See the comments within the function for details.
 */
void *professorthread(void *junk) 
{

  printf("The professor arrived and is starting his office hours\n");

  /* Loop while waiting for students to arrive. */
  while (1) 
  {
    //attempting test casses
    //if class a thread arrives first, exclude all b threads (only A threads)
    //if class b thread arrives first, exclude all a threads (only B threads)

    //use counting semaphore or conditional?



      //pthread_mutex_lock(&lock);
          pthread_mutex_lock(&lock);

      //both consec account for the starvation case, waiting until the 
      //office is clear to restart with a office empty case


      if(classa_consec==5){
        //printf("activated\n");
        empty_flag=1;
         if(students_in_office==0)
         {
           //printf("B going\n");
           //printf("class A in office %d\n",classa_inoffice);
           pthread_cond_signal(&conditionb);
          // printf("class A in office %d\n",classa_inoffice);
           //first_flag=1;
           test=1;
           empty_flag=0;
           //printf("b in office %d",classb_inoffice);
         }
         
         if(first_flag==1&&empty_flag==0)
         {
          test=1;
          printf("class A waiting %d\n\n",classa_waiting);
          pthread_cond_signal(&hold);
          printf("class A in office %d\n",classa_inoffice);
          //pthread_cond_signal(&conditionb);
          //pthread_cond_signal(&conditiona);
          empty_flag=0;
          classa_consec=0;
         }
      }

      /*if(classb_consec==5){
        empty_flag=1;
        test=1;
         if(students_in_office==0)
         {
           pthread_cond_signal(&conditiona);

           empty_flag=0;
         }
         if(first_flag==2&&empty_flag==0)
         {
          test=1;
          printf("class A waiting %d\n\n",classa_waiting);
          pthread_cond_signal(&hold);
          printf("class A in office %d\n",classa_inoffice);
          //pthread_cond_signal(&conditionb);
          //pthread_cond_signal(&conditiona);
          empty_flag=0;
          classa_consec=0;
         }
      }*/

      if(classa_consec>0&&(classb_inoffice>0&&empty_flag==0))
      {
        classa_consec=0;
      }



      //there have been 10 students, and the professor must go on break
      if(students_since_break==professor_LIMIT)
      {
        if(students_in_office==0)
        {
        take_break();
        }

      }
      //important statement, checks if the professor is not on break, and the starvation case is not in effect
      else if(students_since_break<professor_LIMIT&&empty_flag==0)
      {
        //first student arrives or office is empty

        //printf("Office %d\n",students_in_office);

      /*if(test==1)
      {
        printf("Debug 1\n");
        printf("Office %d\n",students_in_office);
        printf("consec_A %d\n",classa_consec);
        printf("class A waiting %d\n",classa_waiting);
        printf("class A in office %d\n",classa_inoffice);
        printf("Empty flag %d\n",empty_flag);
        printf("class B waiting %d\n",classb_waiting);
        printf("class B in office %d\n",classb_inoffice);

      }*/

        if(students_in_office==0){

           /* if(test==1)
             {
               printf("Debug 2\n");
               printf("Office %d\n",students_in_office);
                printf("consec_A %d\n",classa_consec);
                printf("class A waiting %d\n",classa_waiting);
                printf("class A in office %d\n",classa_inoffice);
                printf("Empty flag %d\n",empty_flag);
                printf("class B waiting %d\n",classb_waiting);
                printf("class B in office %d\n",classb_inoffice);

           }*/
           

           if((classa_waiting>classb_waiting||(classb_waiting==0&&classa_waiting>0))&&classb_inoffice==0)
           {

             printf("interior\n\n");
              printf("releasing A\n");
              printf("test %d\n",test);
              printf("class A waiting %d\n",classa_waiting);
              printf("class A in office %d\n",classa_inoffice);
              printf("class B waiting %d\n",classb_waiting);
              
              pthread_cond_signal(&conditiona);
                    
            }
           else if(classb_waiting>0&&classa_inoffice==0)
           {
              printf("no here\n");
              pthread_cond_signal(&conditionb);
           }

        }
        else if(students_in_office>0)
        {

        if(test==1)
      {
        /*printf("Debug 3\n");
        printf("Office %d\n",students_in_office);
        printf("consec_A %d\n",classa_consec);
        printf("class A waiting %d\n",classa_waiting);
        printf("class A in office %d\n",classa_inoffice);
        printf("Empty flag %d\n",empty_flag);
        printf("class B waiting %d\n",classb_waiting);
        printf("class B in office %d\n",classb_inoffice);
        */
      }

          //if there is a class A student in the office, another student A can enter
          if(classa_inoffice!=0)
          {
            pthread_cond_signal(&conditiona);
          }
          else if(classb_inoffice!=0)
          {
            pthread_cond_signal(&conditionb);
          }
        }
      }

     //pthread_mutex_unlock(&lock);
      pthread_mutex_unlock(&lock);
  }
  printf("exiting\n");
  pthread_exit(NULL);
}


/* Code executed by a class A student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */
void classa_enter() 
{
   sem_wait(&class_seatA);

  pthread_mutex_lock(&lock);

      classa_waiting+=1;
      pthread_cond_signal(&arrivea);

      if(classa_consec==5){
        printf("Waiting on threads...\n");
        pthread_cond_wait(&arriveb,&lock);

      }
       printf(" mutex A Locked");
       pthread_cond_wait(&conditiona,&lock);

  if(test==1){
    printf("A student test thread\n");
  }

  students_in_office += 1;
  classa_inoffice += 1;
  classa_consec+=1;
  //students_since_break += 1;

  printf("Students Since Break %d\n",students_since_break);
  printf("Office %d\n",students_in_office);
  printf("consec_A %d\n",classa_consec);
  pthread_mutex_unlock(&lock);


  //process literally kills itself otherwise (assert fails)
  pthread_mutex_lock(&prof_lock);
  students_since_break += 1;
  pthread_mutex_unlock(&prof_lock);
  
}

/* Code executed by a class B student to enter the office.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */
void classb_enter() 
{
    sem_wait(&class_seatB);
      pthread_cond_signal(&arriveb);

   pthread_mutex_lock(&lock);
   classb_consec+=1;
   printf("mutex B locked\n");
   classb_waiting+=1;

   pthread_cond_wait(&conditionb,&lock); 

  students_in_office += 1;
  classb_inoffice += 1;
  //classb_consec+=1;
  //students_since_break += 1;
        printf("Debug B\n");
        printf("Office %d\n",students_in_office);
        printf("consec_A %d\n",classa_consec);
        printf("class A waiting %d\n",classa_waiting);
        printf("class A in office %d\n",classa_inoffice);
        printf("Empty flag %d\n",empty_flag);
        printf("class B waiting %d\n",classb_waiting);
        printf("class B in office %d\n",classb_inoffice);
        printf("students since break %d\n",students_since_break);

  pthread_mutex_unlock(&lock);


  //process literally kills itself otherwise (assert fails)
  pthread_mutex_lock(&prof_lock);
  students_since_break += 1;
  pthread_mutex_unlock(&prof_lock);
}

/* Code executed by a student to simulate the time he spends in the office asking questions
 * You do not need to add anything here.  
 */
static void ask_questions(int t) 
{
  sleep(t);
}


/* Code executed by a class A student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */

//professor lock used, after that the relevant condition is unlocked after
//and used in the professor thread, so that the class is empty and the professor
//can take a break
static void classa_leave() 
{
  pthread_mutex_lock(&prof_lock);

  students_in_office -= 1;
  classa_inoffice -= 1;
  classa_waiting-=1;
  first_flag=0;
  //pthread_cond_signal(&condition);
  pthread_mutex_unlock(&prof_lock);
  
  sem_post(&class_seatA);

}

/* Code executed by a class B student when leaving the office.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */
static void classb_leave() 
{
  /* 
   *  TODO
   *  YOUR CODE HERE. 
   */
  pthread_mutex_lock(&prof_lock);

  students_in_office -= 1;
  classb_inoffice -= 1;
  classb_waiting-=1;
  first_flag=1;
  //pthread_cond_signal(&condition);
  pthread_mutex_unlock(&prof_lock);
  
  sem_post(&class_seatB);


}

/* Main code for class A student threads.  
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classa_student(void *si) 
{
  student_info *s_info = (student_info*)si;

  /* enter office */
  classa_enter();

  printf("Student %d from class A enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classb_inoffice == 0 );
  
  /* ask questions  --- do not make changes to the 3 lines below*/
  printf("Student %d from class A starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class A finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office */
  classa_leave();  

  printf("Student %d from class A leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);

  pthread_exit(NULL);
}

/* Main code for class B student threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* classb_student(void *si) 
{
  student_info *s_info = (student_info*)si;

  /* enter office */
  classb_enter();

  printf("Student %d from class B enters the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);
  assert(classa_inoffice == 0 );

  printf("Student %d from class B starts asking questions for %d minutes\n", s_info->student_id, s_info->question_time);
  ask_questions(s_info->question_time);
  printf("Student %d from class B finishes asking questions and prepares to leave\n", s_info->student_id);

  /* leave office */
  classb_leave();        

  printf("Student %d from class B leaves the office\n", s_info->student_id);

  assert(students_in_office <= MAX_SEATS && students_in_office >= 0);
  assert(classb_inoffice >= 0 && classb_inoffice <= MAX_SEATS);
  assert(classa_inoffice >= 0 && classa_inoffice <= MAX_SEATS);

  pthread_exit(NULL);
}

/* Main function sets up simulation and prints report
 * at the end.
 * GUID: 355F4066-DA3E-4F74-9656-EF8097FBC985
 */
int main(int nargs, char **args) 
{
  int i;
  int result;
  int student_type;
  int num_students;
  void *status;
  pthread_t professor_tid;
  pthread_t student_tid[MAX_STUDENTS];
  student_info s_info[MAX_STUDENTS];

  if (nargs != 2) 
  {
    printf("Usage: officehour <name of inputfile>\n");
    return EINVAL;
  }

  num_students = initialize(s_info, args[1]);
  if (num_students > MAX_STUDENTS || num_students <= 0) 
  {
    printf("Error:  Bad number of student threads. "
           "Maybe there was a problem with your input file?\n");
    return 1;
  }

  printf("Starting officehour simulation with %d students ...\n",
    num_students);

  result = pthread_create(&professor_tid, NULL, professorthread, NULL);

  if (result) 
  {
    printf("officehour:  pthread_create failed for professor: %s\n", strerror(result));
    exit(1);
  }

  for (i=0; i < num_students; i++) 
  {

    s_info[i].student_id = i;
    sleep(s_info[i].arrival_time);
                
    student_type = random() % 2;

    if (s_info[i].class == CLASSA)
    {
      result = pthread_create(&student_tid[i], NULL, classa_student, (void *)&s_info[i]);
    }
    else // student_type == CLASSB
    {
      result = pthread_create(&student_tid[i], NULL, classb_student, (void *)&s_info[i]);
    }

    if (result) 
    {
      printf("officehour: thread_fork failed for student %d: %s\n", 
            i, strerror(result));
      exit(1);
    }
  }

  /* wait for all student threads to finish */
  for (i = 0; i < num_students; i++) 
  {
    pthread_join(student_tid[i], &status);
  }

  /* tell the professor to finish. */
  pthread_cancel(professor_tid);

  printf("Office hour simulation done.\n");

  return 0;
}
