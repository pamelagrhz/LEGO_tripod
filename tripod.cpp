#define STEP_PAUSE 80
// Definiendo las extremidades del Tripod
#define LEFT_LEG OUT_C
#define REAR_LEG OUT_B
#define RIGHT_LEG OUT_A
#define EYE IN_4
// Definiendo Direcciones
#define NEAR 25
#define NONE -1
#define LEFT 1
#define RIGHT 2
#define FRONT 3
#define BACK 4

#define CW 1
#define CCW 0

#define KP 30
#define KD 10
#define KI 0
#define DAMP 60
#define SCALE 5

#define MID 60

int ref[3];

//Ajuste de Extremidad (Piernas)
void Set(byte rear, byte left, byte right)
{
   ref[REAR_LEG] = rear;
   ref[LEFT_LEG] = left;
   ref[RIGHT_LEG] = right;
}

void Pulse(byte motor, int times, bool long_pulse)
{
   repeat (times)
   {
      ref[motor] = MID+10;
      Wait(80);
      //Wait(70+long_pulse*30);
      ref[motor] = MID-10;
      Wait(80);
   }
}

void PulseDouble(byte motor_one, byte motor_two, int times, bool long_pulse)
{
   repeat (times)
   {
      ref[motor_one] = 80;
      ref[motor_two] = 80;
      Wait(50+long_pulse*40);
      ref[motor_one] = 40;
      ref[motor_two] = 40;
   }
}

// Funcion de avanzar 
void Walk (int dir, int times)
{
   repeat(times)
   {
      //Si se dirige al frente
      if (dir == FRONT)
      {
         Set(MID-20,MID,MID);
         Wait(300);
         Set(MID+10,MID,MID);
         Wait(200);
      }
      //Si se dirige hacia atras
      else if (dir == BACK)
      {
         Set(MID-10,MID+20,MID+20);
         Wait(150);
         Set(MID-10,MID-10,MID-10);
         Wait(300);
      }
      //Si se dirige hacia la derecha
      else if (dir == RIGHT)
      {
         Set(MID-10,MID-20,MID-10);
         Wait(80);
         Set(MID+10,MID-20,MID+10);
         Wait(150);
         Set(MID-10,MID-20,MID-10);
         Wait(300);
      }
      // O si se dirige a la izquierda 
      else if (dir == LEFT)
      {
         Set(MID-10,MID-10,MID-20);
         Wait(80);
         Set(MID+10,MID+10,MID-20);
         Wait(150);
         Set(MID-10,MID-10,MID-20);
         Wait(300);
      }
//      if (times>1)  Wait(STEP_PAUSE);
   }
}

// Funciones para regresar en caso de obstaculo
void Turn(int wise, int times)
{
   repeat (times)
   {
      ref[REAR_LEG] = MID-10;
      ref[wise==CCW? RIGHT_LEG:LEFT_LEG] = MID+10;
      Wait(100);
      ref[wise==CCW? LEFT_LEG:RIGHT_LEG] = MID+10;
      ref[wise==CCW? RIGHT_LEG:LEFT_LEG] = MID-10;
      Wait(100);
      ref[wise==CCW? LEFT_LEG:RIGHT_LEG] = MID-10;
      ref[REAR_LEG] = MID+10;
      Wait(100);
//      Pulse( wise==CCW? REAR_LEG:LEFT_LEG, 1, false);
//      Pulse(RIGHT_LEG,1,false);
//      Pulse( wise==CCW? LEFT_LEG:REAR_LEG,1,false);
   }
}

void Demo()
{
   Walk(FRONT,10);
   Turn(CCW,10);
   Walk(FRONT,10);
   Turn(CW,10);
   Walk(FRONT,10);
   Walk(LEFT,10);
   Walk(RIGHT,10);
}

// Control del motor A
task motorAcontroller()
{
   int err, P, I, D, PID, Apos, Aold;
   ResetAllTachoCounts(OUT_A);
   ResetRotationCount(OUT_A);
   while(true)
   {
      Apos = MotorRotationCount(OUT_A);
      err = Apos - ref[OUT_A];
      P = KP*err;
      I = err + KI*I*(100-DAMP)/100;
      D = KD*(err-Aold);
      PID = (P + I + D)/SCALE;
      PID = abs(PID)>100? sign(PID)*100 : PID;
      OnRev(OUT_A,PID);
      Aold = err;
   }
}

// Control del motor B
task motorBcontroller()
{
   int err, P, I, D, PID, Bpos, Bold;
   ResetAllTachoCounts(OUT_B);
   ResetRotationCount(OUT_B);
   while(true)
   {
      Bpos = MotorRotationCount(OUT_B);
      err = Bpos - ref[OUT_B];
      P = KP*err;
      I = err + KI*I*(100-DAMP)/100;
      D = KD*(err-Bold);
      PID = (P + I + D)/SCALE;
      PID = abs(PID)>100? sign(PID)*100 : PID;
      OnRev(OUT_B,PID);
      Bold = err;
   }
}

// Control del motor C
task motorCcontroller()
{
   int err, P, I, D, PID, Cpos, Cold;
   ResetAllTachoCounts(OUT_C);
   ResetRotationCount(OUT_C);
   while(true)
   {
      Cpos = MotorRotationCount(OUT_C);
      err = Cpos - ref[OUT_C];
      P = KP*err;
      I = err + KI*I*(100-DAMP)/100;
      D = KD*(err-Cold);
      PID = (P + I + D)/SCALE;
      PID = abs(PID)>100? sign(PID)*100 : PID;
      OnRev(OUT_C,PID);
      Cold = err;
   }
}

void triped_init()
{
   //SetSensorLowspeed(LEFT_EYE);
   //SetSensorLowspeed(RIGHT_EYE);
   SetSensorLowspeed(EYE);
   OnRev(OUT_ABC,20);
   Wait(80);
   Float(OUT_ABC);
   Wait(500);
   start motorAcontroller;
   start motorBcontroller;
   start motorCcontroller;
   //Set(MID+40,MID+40,MID+40);
   //Wait(1000);
   Set(MID,MID,MID);
   Wait(1000);
   /*
   repeat(3)
   {
      Set(MID,MID+20,MID+20);
      Wait(200);
      Set(MID+20,MID+20,MID);
      Wait(200);
      Set(MID+20,MID,MID+20);
      Wait(600);
   }
   Set(MID,MID,MID);
   */
}

//Brincos
void Jump( int times )
{
   repeat (times)
   {
      Set(90,90,90);
      Wait(1000);
      for (int i=90; i >=30; i--)
      {
         Set(i,i,i);
         Wait(1);
      }
      Wait(1000);
   }
}

task main()
{
   int count;
   triped_init();
   while (true)
   {
      if (ButtonPressed(BTNCENTER,true))
      {
         Set(MID+50,MID+50,MID+50);
         while(ButtonPressed(BTNCENTER,true));
         until(ButtonPressed(BTNCENTER,true));
      }
      if (SensorUS(EYE)>NEAR)
      {
         Walk(FRONT,1);
      }
      else
      {
         Walk(BACK,3);
         count = 0;
         while(SensorUS(EYE)<=NEAR && count<20)
         {
            Turn(CCW,3);
            count++;
         }
         if (SensorUS(EYE)<=NEAR)
         {
            count = 0;
            while(SensorUS(EYE)<NEAR && count<20)
            {
               Turn(CW,3);
               count++;
            }
            Walk(LEFT,4);
         }
         else
         {
            Walk(RIGHT,4);
         }
         //Turn(CCW,5);
      }
   }
}
