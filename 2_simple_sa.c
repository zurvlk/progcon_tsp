#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


float distance(int *point1, int *point2)
{
  int
    dx = point1[0] - point2[0],
    dy = point1[1] - point2[1];

  return sqrt(dx * dx + dy * dy);
}


float totalDistance(int **points, int length)
{
  int
    i;
  float
    total = 0.0;

  for (i = 1; i < length; i++) {
    total += distance(points[i], points[i - 1]);
  }

  total += distance(points[0], points[length - 1]);

  return total;
}


void swap(int **array, int index1, int index2)
{
  int
    *tmp;

  tmp = array[index1];
  array[index1] = array[index2];
  array[index2] = tmp;
}


float frand()
{
  return (float)rand()/(float)RAND_MAX;
}


int shouldChange(float delta, float t)
{
  if (delta <= 0) return 1;
  if (frand() < exp(- delta / t)) return 1;
  return 0;
}


void sa(int **route,
        int numberOfCities,
        float initialT,
        float finalT,
        int n,
        float coolingRate)
{
  int
    currentIterations,
    randomIndex1,
    randomIndex2,
    i;
  float
    t,
    currentTotalDistance,
    newTotalDistance;

  currentTotalDistance = totalDistance(route, numberOfCities);

  for (t = initialT; t > finalT; t *= coolingRate) {
    for (i = 0; i < n; i++) {
      randomIndex1 = rand() % numberOfCities;
      randomIndex2 = rand() % numberOfCities;

      swap(route, randomIndex1, randomIndex2);

      newTotalDistance = totalDistance(route, numberOfCities);

      if (shouldChange(newTotalDistance - currentTotalDistance, t)) {
        currentTotalDistance = newTotalDistance;
      } else {
        swap(route, randomIndex1, randomIndex2);
      }
    }
  }

}


int **buildRoute(int mapData[][2], int numberOfCities)
{
  int
    **route = calloc(numberOfCities, sizeof(&mapData[0][0])),
    i;

  for (i = 0; i < numberOfCities; i++) {
    route[i] = mapData[i];
  }
  return route;
}


void printJSONRoute(int **route, int numberOfCities)
{
  int
    i;

  printf("[\n");
  for (i = 0; i < numberOfCities - 1; i++) {
    printf("  [%d, %d],\n", route[i][0], route[i][1]);
  }
  printf("  [%d, %d]\n", route[i][0], route[i][1]);
  printf("]\n");
}


int main(void)
{
  int
    n = 1000,
    numberOfCities = 51,
    mapData[][2] = {{37,52},{49,49},{52,64},{20,26},{40,30},{21,47},{17,63},{31,62},{52,33},{51,21},{42,41},{31,32},{5,25},{12,42},{36,16},{52,41},{27,23},{17,33},{13,13},{57,58},{62,42},{42,57},{16,57},{8,52},{7,38},{27,68},{30,48},{43,67},{58,48},{58,27},{37,69},{38,46},{46,10},{61,33},{62,63},{63,69},{32,22},{45,35},{59,15},{5,6},{10,17},{21,10},{5,64},{30,15},{39,10},{32,39},{25,32},{25,55},{48,28},{56,37},{30,40}},
    **route = buildRoute(mapData, numberOfCities);
  float
    initialT = 100.0,
    finalT = 0.8,
    coolingRate = 0.9;

  srand((unsigned)time(NULL));

  printf("%f\n", totalDistance(route, numberOfCities));

  sa(route, numberOfCities, initialT, finalT, n, coolingRate);

  printf("%f\n", totalDistance(route, numberOfCities));
  printJSONRoute(route, numberOfCities);

  return 0;
}
