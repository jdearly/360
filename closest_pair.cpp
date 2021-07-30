/*--------------------------------------------------------------------------
-- Author: Joshua Early
-- Course: ITEC 360 - Data Structures and Algorithms
-- Date: March 25, 2018
--
-- The following commands will be required to compile and run this program.
-- The program has been compiled and tested on RUCS.
--
-- To compile:
-- g++ closest_pair.cpp -o closest_pair -std=c++11
--
-- To run the program, a command line argument must be provided.
-- Possible arguments are [BRUTE | DIVIDE | BOTH]
--
-- Run:
-- ./closest_pair [BRUTE | DIVIDE | BOTH] < standard_input_file.txt
--
--
-- Expected behavior:
-- This program has been tested with datasets of size 6 to 1_000_000.
-- Both algorithms (bruteforce, divide/conquer) work on all tested datasets.
-------------------------------------------------------------------------*/

#include <vector>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <float.h>
#include <ctime>
// evil global variables to track the total recursive and distance calls
int recursive_calls = 0;
int distance_calls = 0;
double time_to_read;
float time_to_sort;
double time_to_find;
// struct defined to represent a 2D point
struct point {
	long long int x, y;
};

// vector enum, saves typing
typedef std::vector<point> points;

// declaring a print function to save duplicating code later
void print_points(point arr[], int n) {for(int i = 0; i < n; i++) std::cout
				 << "Point " << i+1 << ": (" << arr[i].x << ", "
				 << arr[i].y << ")" << std::endl; std::cout << std::endl;}

/*----------------------------------------------------------
-- Purpose: Calculates the distance between any given points.
-- Parameters: p1, p2: Points used in calculation.
-- Precondition: Points are integer values
-- Postcondition: Returns the distance between p1 and p2
----------------------------------------------------------*/
float distance(point p1, point p2)
{
	distance_calls++;
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) +
				(p1.y - p2.y)*(p1.y - p2.y));
}

/*----------------------------------------------------------
-- Purpose: Brute-force approach to finding the closest pair.
-- Parameters: p, n, closest_pair[]
-- Precondition: Vector p is not empty.
-- Postcondition: Returns the minimum distance between points.
----------------------------------------------------------*/
float brute_force(points p, int n, point closest_pair[])
{
	point point1, point2;
	float min = FLT_MAX;
	for (int i = 0; i < n; ++i)
		for (int j = i+1; j < n; ++j)
			if (distance(p.at(i), p.at(j)) < min){
				min = distance(p.at(i), p.at(j));
				closest_pair[0] = p.at(j);
				closest_pair[1] = p.at(i);
			}
	return min;
}

/*----------------------------------------------------------
-- Purpose: Check points until distance between y < d
-- Parameters: strip, d, closest_pair[]
-- Precondition: strip is populated with x and y pairs, strip > 1
-- Postcondition: Returns the current minimum distance.
-- O(n) operation
----------------------------------------------------------*/
float closest_strip(points strip, float d, point closest_pair[])
{
	d = FLT_MAX;
	float min = d;
	for (int i = 0; i < strip.size(); ++i)
		for (int j = i+1; j < strip.size() &&
				(strip.at(j).y - strip.at(i).y) < min; j++){
			if (distance(strip.at(i), strip.at(j)) < min) {
				closest_pair[0] = strip.at(j); // saving current closest pair
				closest_pair[1] = strip.at(i);
				min = distance(strip.at(i), strip.at(j)); //record current min
			}
		}
	return min;
}

/*----------------------------------------------------------
-- Purpose: Recursive function to divide and conquer the given
-- dataset. Finds the smallest distance between points in a 2D plane.
-- Parameters: pt_x, pt_y, closest_pair[]
-- Precondition: pt_x and pt_y are populated with x and y pairs.
-- pt_x and pt_y are sorted
-- Postcondition: Returns the minimum distance and closest pair.
----------------------------------------------------------*/
float closest_sorted(points pt_x, points pt_y, point closest_pair[]) {
	++recursive_calls;
	if (pt_x.size() <= 3)
		return closest_strip(pt_x, pt_x.size(), closest_pair);

	// find and set the midpoint
	auto mid_point = pt_x.size() / 2;

	points pt_y_left, pt_y_right;

	for (auto p : pt_y) {
		if (p.x <= pt_x[mid_point].x) pt_y_left.push_back(p);
		else pt_y_right.push_back(p);
	}

	//
	points pt_x_left = points(pt_x.begin(), pt_x.begin() + mid_point);
	points pt_x_right = points(pt_x.begin() + mid_point + 1, pt_x.end());

	// smallest distance on the left and right of the mid_point
	auto dist_left = closest_sorted(pt_x_left, pt_y_left, closest_pair);
	auto dist_right = closest_sorted(pt_x_right, pt_y_right, closest_pair);

	// smallest distance of both sides
	auto lr_min = std::min(dist_left, dist_right);

	points strip; // vector for points close to the line

	for (auto p : pt_y)
	if (p.x - pt_x[mid_point].x < lr_min) strip.push_back(p);

	// return the closest pair in the strip vector
	return std::min(lr_min, closest_strip(strip, lr_min, closest_pair));
}

/*----------------------------------------------------------
-- Purpose: Copies point vector to sort both x and y values
-- Parameters: p, closest_pair[]
-- Precondition: vector p is populated with x and y pairs
-- Postcondition: Calls closest_sorted recursive function and
-- returns to closest pair distance.
-- Allows for the vectors to be sorted once, rather than
-- each time the recursive call is made.
----------------------------------------------------------*/
float closest(points p, point closest_pair[]) {
	points pt_x = p, pt_y = p;
	clock_t begin = clock();
	std::sort(pt_x.begin(), pt_x.end(), [](point p1, point p2)
									 { return p1.x < p2.x; });
	std::sort(pt_y.begin(), pt_y.end(), [](point p1, point p2)
									 { return p1.y < p2.y; });

	clock_t end = clock();
	time_to_sort = double(end - begin) / (CLOCKS_PER_SEC / 100);

	clock_t begin_find = clock();
	float closest = closest_sorted(pt_x, pt_y, closest_pair);
	clock_t end_find = clock();
	time_to_find = double(end_find - begin_find) / (CLOCKS_PER_SEC / 100);
	return closest;
}

/*----------------------------------------------------------
-- Purpose: Responsible for printing divide and conquer output
-- Parameters: p, n, closest_pair[]
-- Precondition: vector p has been populated
-- Postcondition: Prins divide and conquer data to standard output
----------------------------------------------------------*/
int divide_conquer_output(points p, int n, point closest_pair[])
{
	distance_calls = 0;
	float dist = closest(p, closest_pair);
	std::cout << "* * * * * * * * * * * * * * * * *" << std::endl;
	std::cout << "* Algorithm: Divide and Conquer *" << std::endl;
	std::cout << "* * * * * * * * * * * * * * * * *" << std::endl << std::endl;
	std::cout << "N: " << n << std::endl << std::endl;
	print_points(closest_pair, 2);
	std::cout << "Distance: " << dist << std::endl << std::endl;
	std::cout << "Number of distance calcs: " << distance_calls << std::endl;
	std::cout << "Number of calls: " << recursive_calls << std::endl;
	std::cout << "Time to read: " << time_to_read << std::endl;
	std::cout << "Time to sort: " << time_to_sort << std::endl;
	std::cout << "Time to find: " << time_to_find << std::endl;

}

/*----------------------------------------------------------
-- Purpose: Responsible for printing bruteforce output data
-- Parameters: p, n, closest_pair[]
-- Precondition: vector p has been populated
-- Postcondition: Prints bruteforce data to standard output
----------------------------------------------------------*/
int brute_output (points p, int n, point closest_pair[])
{
	distance_calls = 0;
	float dist =  brute_force(p, n, closest_pair);
	std::cout << "* * * * * * * * * * * * *" << std::endl;
	std::cout << "* Algorithm: Bruteforce *" << std::endl;
	std::cout << "* * * * * * * * * * * * *" << std::endl << std::endl;
	std::cout << "N: " << n << std::endl << std::endl;
	print_points(closest_pair, 2);
	std::cout << "Distance: " << dist  << std::endl << std::endl;
	std::cout << "Number of distance calcs: " << distance_calls << std::endl;
	std::cout << std::endl;
}

/*----------------------------------------------------------
-- Purpose: Main driver function for the program.
-- Parameters: argc, argv[]
-- Precondition: None
-- Postcondition: Calls appropriate algorithm based on given
-- argument.
----------------------------------------------------------*/
int main(int argc, char* argv[]) {

	std::cout.precision(5);
	std::cout << std::fixed;
	long long int n, x, y;
	std::cin >> n;
	points points;
	point closest_pair[2];

	clock_t begin = std::clock();
	for (int i = 0; i < n; i++) {
		std::cin >> x >> y;
		point p;
		p.x = x;
		p.y = y;
		points.push_back(p);
	}
	clock_t end = std::clock();
  time_to_read = float(end - begin) / (CLOCKS_PER_SEC / 100);

	std::string brute = "brute";
	std::string div = "divide";
	std::string both = "both";

	if (argv[1] != NULL) {
		if (strcasecmp(std::string(argv[1]).c_str(),brute.c_str()) == 0) {
			brute_output(points, n, closest_pair);
		}
		else if (strcasecmp(std::string(argv[1]).c_str(),div.c_str()) == 0) {
			divide_conquer_output(points, n, closest_pair);
		}
		else if (strcasecmp(std::string(argv[1]).c_str(),both.c_str()) == 0) {
			brute_output(points, n, closest_pair);
			divide_conquer_output(points, n, closest_pair);
		}
		else
			std::cout << "Invalid argument. Valid keywords: "
					  << "[BRUTE | DIVIDE | BOTH]"
					  << std::endl;
	}
	else
		std::cout << "Please provide command line argument. "
				  << "Valid keywords: [BRUTE | DIVIDE | BOTH]"
				  << std::endl;
	return 0;
}
