#include <stdio.h>
#include <math.h>
#define N 4
#define EPSILON 0.001
#define X 10
double calculate_distance(double, double, double, double);
double update_center_point(double, double, int);
int main()
{
    double x_coordinate_of_the_center_of_cluster1, y_coordinate_of_the_center_of_cluster1,
        x_coordinate_of_the_center_of_cluster2, y_coordinate_of_the_center_of_cluster2,
        x_coordinate_of_the_center_of_cluster3, y_coordinate_of_the_center_of_cluster3,
        x_coordinate, y_coordinate, closest_one, next_closest_one, distance1, distance2, distance3, difference,
        old_x_cluster1, old_y_cluster1, old_x_cluster2, old_y_cluster2, old_x_cluster3, old_y_cluster3;
    int number_of_points_cluster1 = 0, number_of_points_cluster2 = 0, number_of_points_cluster3 = 0,
        number_of_points_all_clusters = 0;
    /*Program accepts center points for three clusters*/
    scanf("%lf %lf", &x_coordinate_of_the_center_of_cluster1, &y_coordinate_of_the_center_of_cluster1);
    scanf("%lf %lf", &x_coordinate_of_the_center_of_cluster2, &y_coordinate_of_the_center_of_cluster2);
    scanf("%lf %lf", &x_coordinate_of_the_center_of_cluster3, &y_coordinate_of_the_center_of_cluster3);
    /*Program  continues to capture point coordinates*/
    while (number_of_points_all_clusters != N)
    {
        old_x_cluster1 = x_coordinate_of_the_center_of_cluster1;
        old_y_cluster1 = y_coordinate_of_the_center_of_cluster1;
        old_x_cluster2 = x_coordinate_of_the_center_of_cluster2;
        old_y_cluster2 = y_coordinate_of_the_center_of_cluster2;
        old_x_cluster3 = x_coordinate_of_the_center_of_cluster3;
        old_y_cluster3 = y_coordinate_of_the_center_of_cluster3;
        scanf("%lf %lf", &x_coordinate, &y_coordinate);
        /*Program calculating distances between points and three clusters*/
        distance1 = calculate_distance(x_coordinate, y_coordinate, x_coordinate_of_the_center_of_cluster1, y_coordinate_of_the_center_of_cluster1);
        distance2 = calculate_distance(x_coordinate, y_coordinate, x_coordinate_of_the_center_of_cluster2, y_coordinate_of_the_center_of_cluster2);
        distance3 = calculate_distance(x_coordinate, y_coordinate, x_coordinate_of_the_center_of_cluster3, y_coordinate_of_the_center_of_cluster3);
        /*Program finding closest value and and next closest value*/
        if (distance1 <= distance2 && distance1 <= distance3)
        {
            closest_one = distance1;
            if (distance2 <= distance3)
            {
                next_closest_one = distance2;
            }
            else
                next_closest_one = distance3;
        }
        else if (distance2 <= distance1 && distance2 <= distance3)
        {
            closest_one = distance2;
            if (distance1 <= distance3)
            {
                next_closest_one = distance1;
            }
            else
                next_closest_one = distance3;
        }
        else if (distance3 <= distance1 && distance3 <= distance2)
        {
            closest_one = distance3;
            if (distance1 <= distance2)
            {
                next_closest_one = distance1;
            }
            else
                next_closest_one = distance2;
        }
        /*Program is going to decide which cluster a point to belongs to*/
        /*Program is going to choose the closest cluster if there is at least X percent
        difference between the closest one and the next closest*/
        /*If there is less than X percent difference, program is going to discard the point*/
        if (closest_one == 0)
        {
            closest_one += EPSILON; /*Program avoids divide by zero situation*/
        }
        difference = (double)((next_closest_one - closest_one) / closest_one) * 100; /*calculating percent difference*/
        if (difference >= X)
        {
            if (closest_one == EPSILON) /*by doing this providing previous value*/
                closest_one = 0;
            if (closest_one == distance1)
            {
                ++number_of_points_cluster1;
                printf("Point %lf, %lf was included in Cluster 1\n", x_coordinate, y_coordinate);
                x_coordinate_of_the_center_of_cluster1 = update_center_point(old_x_cluster1, x_coordinate, number_of_points_cluster1);
                y_coordinate_of_the_center_of_cluster1 = update_center_point(old_y_cluster1, y_coordinate, number_of_points_cluster1);
            }
            else if (closest_one == distance2)
            {
                ++number_of_points_cluster2;
                printf("Point %lf, %lf was included in Cluster 2\n", x_coordinate, y_coordinate);
                x_coordinate_of_the_center_of_cluster2 = update_center_point(old_x_cluster2, x_coordinate, number_of_points_cluster2);
                y_coordinate_of_the_center_of_cluster2 = update_center_point(old_y_cluster2, y_coordinate, number_of_points_cluster2);
            }
            else if (closest_one == distance3)
            {
                ++number_of_points_cluster3;
                printf("Point %lf, %lf was included in Cluster 3\n", x_coordinate, y_coordinate);
                x_coordinate_of_the_center_of_cluster3 = update_center_point(old_x_cluster3, x_coordinate, number_of_points_cluster3);
                y_coordinate_of_the_center_of_cluster3 = update_center_point(old_y_cluster3, y_coordinate, number_of_points_cluster3);
            }
            number_of_points_all_clusters = number_of_points_cluster1 + number_of_points_cluster2 + number_of_points_cluster3;
        }
        /* if difference is less than X discard the point*/
        else
        {
            printf("Point %lf, %lf was discarded\n", x_coordinate, y_coordinate);
        }
    }
    /*It prints out center coordinates of the cluster and number of clusters*/
    /*Program is adding central points as well by doing + 1*/
    printf("Cluster 1 : %lf %lf %d\n", x_coordinate_of_the_center_of_cluster1, y_coordinate_of_the_center_of_cluster1, number_of_points_cluster1 + 1);
    printf("Cluster 2 : %lf %lf %d\n", x_coordinate_of_the_center_of_cluster2, y_coordinate_of_the_center_of_cluster2, number_of_points_cluster2 + 1);
    printf("Cluster 3 : %lf %lf %d\n", x_coordinate_of_the_center_of_cluster3, y_coordinate_of_the_center_of_cluster3, number_of_points_cluster3 + 1);
    return (0);
}
/*This function calculates distance bewtween two points*/
double calculate_distance(double x1, double y1, double x2, double y2)
{
    double distance = sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
    return distance;
}
/*This function updates center point.I wrote that for x axis but it is also valid for y axis*/
double update_center_point(double old_x, double point_x, int number_of_points)
{
    double new_x = (old_x * number_of_points + point_x) / (number_of_points + 1);
    return new_x;
}