#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define RES_SIZE 4
#define ITER 5
#define RTParam 5

#define printff(args...) printf(args); fflush_SERIAL()
#define printlnf(x) println(x); Serial.flush()

int serial_putchar(char c, FILE* f) {
   if (c == '\n') serial_putchar('\r', f);
   return Serial.write(c) == 1? 0 : 1;
}

FILE serial_stdout;

typedef struct resurs{
	UBaseType_t lock;
	SemaphoreHandle_t mutex;
	UBaseType_t maxPr;
	int val;
} resurs;

static UBaseType_t glMaxPr;
static UBaseType_t curTask;

resurs resursi[RES_SIZE];
SemaphoreHandle_t prioMutexi[configMAX_PRIORITIES];

void TaskBlink0( void *pvParameters );
void TaskBlink1( void *pvParameters );
void TaskBlink2( void *pvParameters );

static void lock(int i, UBaseType_t pr, UBaseType_t tsk){
	Serial.print(F("lock! i:")); printf("%d pr:%u tsk:%x |%u\n",i,pr,tsk,xTaskGetTickCount());
	Serial.flush();
	if(pr <= glMaxPr && tsk != curTask){
		xSemaphoreTake(prioMutexi[glMaxPr],portMAX_DELAY);
		xSemaphoreGive(prioMutexi[glMaxPr]);
	}
	xSemaphoreTake(resursi[i].mutex, portMAX_DELAY);
	if(resursi[i].maxPr > glMaxPr){
		glMaxPr = resursi[i].maxPr;
		xSemaphoreTake(prioMutexi[glMaxPr],portMAX_DELAY);
		curTask=tsk;

	}
	resursi[i].lock = 1;
}

static void unlock(int i, UBaseType_t tsk){
	Serial.print(F("Unlock! i:")); printf("%d |%u\n",i,xTaskGetTickCount());
	Serial.flush();
	xSemaphoreGive(resursi[i].mutex);
	resursi[i].lock = 0;
	xSemaphoreGive(prioMutexi[glMaxPr]);
	for(i=0;i<RES_SIZE;i++){
		if(resursi[i].lock && resursi[i].maxPr > glMaxPr){
			glMaxPr = resursi[i].maxPr;
			curTask = tsk;
		}
	}
}

static void work(int x){
	TickType_t startCount = xTaskGetTickCount();
	while(xTaskGetTickCount() < startCount + RTParam * x);
}

void setup() {
	Serial.begin(115200); //115200
	while (!Serial);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &serial_stdout;
	stderr = stdout;
	Serial.printlnf(F("Hello from setup()!"));

	for(int i=0;i<RES_SIZE;i++){
		resursi[i].mutex = xSemaphoreCreateMutex();
	}

	for(int i=0;i<configMAX_PRIORITIES;i++){
		prioMutexi[i] = xSemaphoreCreateMutex();
	}

	xTaskCreate(TaskBlink0, "", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(TaskBlink1, "", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(TaskBlink2, "", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

void loop()
{

}

void TaskBlink0(void *pvParameters)
{
	Serial.print(F("Hello form task0!")); printf(" |%u\n",xTaskGetTickCount());
	digitalWrite(LED_BUILTIN, HIGH);

	work(1); //init
	lock(3, uxTaskPriorityGet(0), 0); //C
	for(int i=0;i<ITER;i++) resursi[3].val++;
	work(1); //-> II

	Serial.print(F("Hello form task0!")); printf(" |%u\n",xTaskGetTickCount());
	for(int i=0;i<ITER;i++) resursi[3].val++; //C2
	work(1);

	lock(2, uxTaskPriorityGet(0), 0); //B
	for(int i=0;i<ITER;i++) resursi[2].val++;
	work(1); //->III

	Serial.print(F("Hello form task0!")); printf(" |%u\n",xTaskGetTickCount());
	for(int i=0;i<ITER;i++) resursi[2].val++; //B2
	work(1);
	unlock(2,0); //->III

	Serial.print(F("Hello form task0!")); printf(" |%u\n",xTaskGetTickCount());
	work(1);
	for(int i=0;i<ITER;i++) resursi[3].val++; //C3
	unlock(3,0);

	work(1); // ende


	Serial.print(F("Goodbye form task0")); printf(" |%u\n",xTaskGetTickCount());
	vTaskDelete(0);
}

void TaskBlink1(void *pvParameters)
{

	vTaskDelay(10);
	Serial.print(F("Hello form task1")); printf(" |%u\n",xTaskGetTickCount());
	digitalWrite(LED_BUILTIN, LOW);

	work(1);
	lock(3, uxTaskPriorityGet(0), 1); //->I

	Serial.print(F("Hello form task1")); printf(" |%u\n",xTaskGetTickCount());
	for(int i=0;i<ITER;i++) resursi[3].val++;
	work(1); //C4
	unlock(3,1);
	work(1);//-> I

	Serial.print(F("Goodbye form task1")); printf(" |%u\n",xTaskGetTickCount());
	vTaskDelete(0);
}


void TaskBlink2(void *pvParameters)
{
	vTaskDelay(30);
	Serial.print(F("Hello form task2")); printf(" |%u\n",xTaskGetTickCount());
	digitalWrite(LED_BUILTIN, LOW);
	work(1);

	lock(1,uxTaskPriorityGet(0), 2);  // -> I

	Serial.print(F("Hello form task2")); printf(" |%u\n",xTaskGetTickCount());
	work(1);
	for(int i=0;i<ITER;i++) resursi[1].val++; //A

	work(1);
	Serial.print(F("Hello form task2")); printf(" |%u\n",xTaskGetTickCount());

	lock(2,uxTaskPriorityGet(0), 2); //B3

	Serial.print(F("Hello form task2")); printf(" |%u\n",xTaskGetTickCount());

	for(int i=0;i<ITER;i++) resursi[2].val++;
	work(1);
	unlock(2,1);

	Serial.print(F("Goodbye form task2")); printf(" |%u\n",xTaskGetTickCount());
	vTaskDelete(0);
}

