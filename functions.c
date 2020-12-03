#include "functions.h"
#include "structs.h"
#define ROAD_COUNT 3
Vehicle set_car_acceleration(Vehicle car){
  car.acceleration = (2 * (1 - (car.time_driving/car.speed_limit_time)) * (car.speed_limit - 0)) / car.speed_limit_time;
  return car;
}

Vehicle accelerate_car(Vehicle car, Road roads[]){
  car.speed += car.acceleration;
  if (car.speed > car.speed_limit) {
    car.speed = car.speed_limit;
  }
  if (car.speed > roads[car.lane].speed_limit) {
    car.speed = roads[car.lane].speed_limit;
  }
  return car;
}

Vehicle state_waiting(Vehicle car, Vehicle *cars, int cars_int, Road roads[], Traffic_light lights[], int lights_int) {
  Vehicle closest = get_nearest_car(car, cars, cars_int, roads);
  int is_safe = check_if_safe_distance(car, closest);
  if (is_safe == 1) {
   car.state = Driving;
   car = state_driving(car, cars, cars_int, roads, lights, lights_int);
 }
return car;
}

Vehicle state_driving(Vehicle car, Vehicle *cars, int cars_int, Road roads[], Traffic_light lights[], int lights_int) {
  car = set_car_acceleration(car);
  Vehicle closest = get_nearest_car(car, cars, cars_int, roads);
  car = set_safe_distance(car);
  Traffic_light light = nearest_traffic_light(car, lights, lights_int);



  int is_safe = check_if_safe_distance(car, closest);

  if (is_safe == 1) {
      car = accelerate_car(car, roads);
      if (car.state == HoldingForRed) {
        car.state = Driving;
      }
  }

  if (is_safe == 0) {

    if (closest.state == Mock) {
      return car;
    }
      if (car.speed > closest.position - car.position) {
        car.speed = closest.speed;
      }

       // Needs a check if car will end up in other car, if so deaccelerate
  }

  car = check_light(light, car, closest);
  car.position += car.speed;
  if (car.position > 0) {
      car.secs_on_bridge += 1;
  }
  if (car.position > roads[car.lane].length) {
      car.state = Done;
  }
  return car;
}

void prompt(double *thru_put, int *iter_speed){
  do{
    system("cls");
    printf("What thru-put do u want per hour? ");
    scanf(" %lf", &*thru_put);
  }while(*thru_put < 0);

  do{
    system("cls");
    printf("\nWhat iteration speed do u want? (1: real time, 50: fast, 9999999: fast as fuck boi) ");
    scanf(" %d", &*iter_speed);
  }while(*iter_speed < 0);

}


Vehicle drive(Vehicle car, Vehicle *cars, int cars_int, Road roads[], Traffic_light lights[], int lights_int) {
   if (car.state == Waiting) {
car = state_waiting(car, cars, cars_int, roads, lights, lights_int);
   } else if(car.state == Driving) {
     car = state_driving(car, cars, cars_int, roads, lights, lights_int);
   } else if(car.state == HoldingForRed) {
     car = state_driving(car, cars, cars_int, roads, lights, lights_int);
   }
 return car;
}

double ms_to_kmt(double x){
  return x * 3.6;
}


double kmt_to_ms(double x){
  return x / 3.6;
}
Vehicle set_safe_distance(Vehicle car) {
    car.safe_distance = (ms_to_kmt(car.speed) / 2) + 1;
    return car;
}

int check_if_safe_distance(Vehicle car, Vehicle car_in_front) {
    if (car_in_front.state == Mock) {
      return 1;
    }

    double delta = car_in_front.position - car.position -2.2;
    if (delta > car.safe_distance)
    {
      return 1;
    }
    return 0;
}
void print_vehicle(Vehicle car) {
  if (car.state != Done) {
    printf("Vehicle(%d:%s): Speed: %.3lf(%.1lf), position: %.2lf, secs_on_bridge: %d, speed_limit: %.1lf, acceleration: %.3lf, safe_distance: %.2lf, Lane: %d, State: %s\n", car.ID, lane_to_string(car.type),car.speed, ms_to_kmt(car.speed), car.position, car.secs_on_bridge, ms_to_kmt(car.speed_limit), car.acceleration, car.safe_distance, car.lane, state_to_string(car.state));

  }
    }

void print_all_vechile(Vehicle car) {
        if (car.ID % 2 == 1) {
          printf("\n");
        }
        printf("Vehicle(%d:%s): Lane: %d, secs_on_bridge: %d, speed_limit: %.1lf, acceleration: %.3lf, State: %s\n", car.ID,lane_to_string(car.type), car.speed, car.lane, car.secs_on_bridge, ms_to_kmt(car.speed_limit), car.acceleration, state_to_string(car.state));
}

void print_vehicles(Vehicle *cars, int cars_int) {

    for (int i = 0; i < cars_int; i++) {
        print_vehicle(cars[i]);
    }
}

void print_all_vechiles(Vehicle *cars, int cars_int) {

    for (int i = 0; i < cars_int; i++) {
        print_all_vechile(cars[i]);
    }
}

char* color_to_string(Light_color color) {
  switch (color)
  {
  case red:
    return "Red";
    break;
  case green:
    return "Green";
    break;
  case dummy:
    return "Dummy";
    break;
  default:
  return "Not Defined";
    break;
  }
}

char* lane_to_string(Lane_type type) {
  switch (type)
  {
  case Car:
    return "Car";
    break;
  case PlusBus:
    return "Plus Bus";
    break;
  case Bus:
    return "Bus";
    break;
  default:
    return "Not Defined";
    break;
  }
}

char* state_to_string(State state) {
  switch (state)
  {
  case Waiting:
    return "Waiting";
    break;
  case Driving:
    return "Driving";
    break;
  case Done:
    return "Done";
    break;
    case HoldingForRed:
      return "Holding For Red";
      break;
    case Mock:
      return "Mock";
      break;
  default:
  return "Not Defined";
    break;
  }
}

Road create_road(double speed_limit, Lane_type lane, double len) {
    Road road = {kmt_to_ms(speed_limit), lane, len};
    return road;
}

Traffic_light create_light(Light_color color, double position, int timer_green, int timer_red) {
    Traffic_light light = {color, position, 0, timer_green, timer_red};
    return light;
}

Traffic_light count_timer(Traffic_light light) {
  light.timer += 1;
    if (light.color == green && light.timer == light.timer_green)
    {
      light.color = red;
      light.timer = 0;
    }
    else if (light.color == red && light.timer == light.timer_red)
    {
      light.color = green;
      light.timer = 0;
    }
  return light;
}

Traffic_light nearest_traffic_light(Vehicle car, Traffic_light lights[], int lights_int) {
  int i;
  Traffic_light nearest_light = {dummy, 99999, 0, 0, 0};
  if (lights_int < 1) {
    return nearest_light;
  }
  for (i = 0; i < lights_int; i++) { /* remember variable for number of lights */
    if (lights[i].position > car.position && nearest_light.position > lights[i].position) {
      nearest_light = lights[i];
    }
  }
  return nearest_light;
}

Vehicle create_vehicle(int id, int dist, double speed_limit_, Road roads[]) {
    Lane_type type;
    double chance = rand_uniform(0, 100);
    if (chance <= 90) {
      type = Car;
    } else if (chance > 90 && chance < 95) {
      type = PlusBus;
    } else {
      type = Bus;
    } 
    
    int lane;
    int waiting = 0;
    do {
    lane = rand() % ROAD_COUNT; 
    if (roads[lane].lane_type == type) {
      waiting = 1;
    } else if(roads[lane].lane_type == Car && type == Bus) {
      waiting = 1;
    }
    } while (!waiting);
    
    double speed = 0;
    double breaks = 0;
    double position = dist;
    double length = 0;
    double speed_limit = kmt_to_ms(speed_limit_);
    double speed_limit_time = 65.5;
    double time_driving = 0;
    double acceleration = 0;
    double safe_distance = 1;
    int ID = id;
    int secs_on_bridge = 0;
    State state = Waiting;

    Vehicle car = {speed, breaks, position, length, speed_limit, speed_limit_time, time_driving, acceleration, safe_distance, ID, lane, secs_on_bridge, state, type};
    return car;
}

Vehicle get_nearest_car(Vehicle car, Vehicle *cars, int cars_int, Road roads[]) {
    Vehicle closest = create_vehicle(-1, 99999999, 160, roads);
    if (cars_int < 1) {
        return closest;
    }
    for (int i = 0; i < cars_int; i++) {
        if (car.position < cars[i].position && cars[i].position < closest.position && car.lane == cars[i].lane) {
            closest = cars[i];
        }
    }
    return closest;
}

Vehicle create_random_vehicle(int id, Road roads[]) {
  double speed_limit = rand_uniform(170, 250);
  Vehicle car = create_vehicle(id, 0, speed_limit, roads);
  return car;
}

Vehicle * randomize_cars(Vehicle *cars,int m, int n, Road roads[]) {
  int id = m;
  for (int i = m; i < n; i++) {
    id += 1;
    cars[i] = create_random_vehicle(id, roads);
  }
  return cars;
}

Vehicle * Create_allocate_cars(int n, Road roads[]) {
 Vehicle *cars = malloc(sizeof(Vehicle) * n);
 cars = randomize_cars(cars, 0, n, roads);
 return cars;
}

int cmpfunc (const void * a, const void * b) {
   Vehicle l = *(const Vehicle *)a;
   Vehicle r = *(const Vehicle *)b;
   return l.lane - r.lane;
}

void sort_lanes(Vehicle *cars, int cars_int){
  Vehicle *print_cars = malloc(sizeof(Vehicle) * cars_int);
  for (int i = 0; i < cars_int; i++){
    print_cars[i] = cars[i];
  }
  qsort(print_cars, cars_int, sizeof(Vehicle), cmpfunc);
  for (int i = 0; i < cars_int; i++){
    print_all_vechile(print_cars[i]);
  }
  free(print_cars);
}

Vehicle * Realloc_cars(Vehicle *ptr, int *cars_int, int new, Road roads[]) {
  Vehicle *cars = malloc(sizeof(Vehicle) * (*cars_int + new));
  for (int i = 0; i < *cars_int; i++) {
    cars[i] = ptr[i];
  }
  free(ptr);
  int id = *cars_int;
  for (int i = *cars_int; i < *cars_int + new; i++) {
    id += 1;
    cars[i] = create_random_vehicle(id, roads);
  }
  *cars_int += new;
  return cars;
}

Vehicle check_light(Traffic_light light, Vehicle car, Vehicle closest) {
  if (light.color == red) {
    if (light.position - car.position < 30) {
      car.speed = 5;
      if (closest.position - car.position < 10) {
        car.speed = closest.position - car.position - 1;
        if (car.speed < 0) {
          car.speed = 0;
        }
      }
    }
    if (light.position - car.position < 10) {
      car.speed = 0;
    }
    if (car.speed == 0 && car.position > 1) {
      car.state = HoldingForRed;
    }
}
  return car;
}

double rand_uniform(double min, double max){
  return (max - min) * ( (double)rand() / (double)RAND_MAX ) + min;
}

void print_traffic_light(Traffic_light lights[], int a) {
  for (int i = 0; i < a; i++) {
    if (lights[i].position > 1) {
      if (color_to_string(lights[i].color) == "Red") {
      printf("Position: %.0lf Color: %s   Timer: %d/%d\n", lights[i].position, color_to_string(lights[i].color), lights[i].timer, lights[i].timer_red);
      }
      else {
      printf("Position: %.0lf Color: %s Timer: %d/%d\n", lights[i].position, color_to_string(lights[i].color), lights[i].timer, lights[i].timer_green);
      }
    }
    else {
      if (color_to_string(lights[i].color) == "Red") {
      printf("Position:   %.0lf Color: %s   Timer: %d/%d\n", lights[i].position, color_to_string(lights[i].color), lights[i].timer, lights[i].timer_red);
      }
      else {
      printf("Position:   %.0lf Color: %s Timer: %d/%d\n", lights[i].position, color_to_string(lights[i].color), lights[i].timer, lights[i].timer_green);
      }
    }
  }
}
