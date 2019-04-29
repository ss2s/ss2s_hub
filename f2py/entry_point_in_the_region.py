#!/usr/bin/env python
# _*_ coding utf-8 _*_

from math  import sqrt

region_latitude = 50.907483
region_longitude = 34.798629
region_radius = 50  #уе

point_latitude = 0
point_longitude = 0

print("\n\n")
print("region_latitude:", region_latitude)
print("region_longitude:", region_longitude)
print("region_radius:", region_radius)
print("\n\n")
print("enter the coordinats of the point, float:")
print("\n")
point_latitude = input("latitude: ")
print("\n")
point_longitude = input("longitude: ")

point_latitude = float(point_latitude)
point_longitude = float(point_longitude)

if region_latitude > point_latitude:
    side_a = region_latitude - point_latitude
elif region_latitude < point_latitude:
    side_a = point_latitude - region_latitude
else:
    side_a = 0

if region_longitude > point_longitude:
    side_b = region_longitude - point_longitude
elif region_longitude < point_longitude:
    side_b = point_longitude - region_longitude
else:
    side_b = 0

side_c2 = (side_a * side_a) + (side_b * side_b)
side_c = sqrt(side_c2)

print("\n")
print("distance to the point = ", side_c)
print("\n")

if side_c <= region_radius:
    print("point IS INCLUDED in the region tracking")
else:
    print("point NOT INCLUDED in region tracking")

print("\n\n\n")
input("press Enter to exit")