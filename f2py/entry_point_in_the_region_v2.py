#!/usr/bin/env python3
# _*_ coding utf-8 _*_

from math  import radians, asin, sqrt, pow, sin, cos

region_latitude = 50.907483
region_longitude = 34.798629
region_radius = 50.0  #m

point_latitude = 0.0
point_longitude = 0.0

def distanceF(latitudeFrom, longitudeFrom, latitudeTo, longitudeTo, earthRadius = 6371000):
	# convert from degrees to radians
	latFrom = radians(latitudeFrom)
	lonFrom = radians(longitudeFrom)
	latTo = radians(latitudeTo)
	lonTo = radians(longitudeTo)

	latDelta = latTo - latFrom
	lonDelta = lonTo - lonFrom

	angle = 2 * asin(sqrt(pow(sin(latDelta / 2), 2) + cos(latFrom) * cos(latTo) * pow(sin(lonDelta / 2), 2)))

	return angle * earthRadius

print("\n\n")
print("region_latitude:", region_latitude)
print("region_longitude:", region_longitude)
print("region_radius:", region_radius, "m")
print("\n\n")
print("enter the coordinats of the point, float: 00.000000")
print("\n")
point_latitude = input("latitude: ")
print("\n")
point_longitude = input("longitude: ")

point_latitude = float(point_latitude)
point_longitude = float(point_longitude)

distanceV = distanceF(region_latitude, region_longitude, point_latitude, point_longitude)

print("\n")
print("distance to the point = ", distanceV)
print("\n")

if distanceV <= region_radius:
    print("point IS INCLUDED in the region tracking")
else:
    print("point NOT INCLUDED in region tracking")

print("\n\n\n")
input("press Enter to exit")