#pragma once
#include <math.h>
#include <stdbool.h>

static inline bool valid_coordinates(double lat, double lon) {
  return isfinite(lat) && isfinite(lon) && lat >= -90.0 && lat <= 90.0 &&
         lon >= -180.0 && lon <= 180.0;
}

static inline double distance_m(double lat1, double lon1, double lat2, double lon2) {
  const double rad = 0.017453292519943295;
  const double dlat = (lat2 - lat1) * rad;
  const double dlon = (lon2 - lon1) * rad;
  double a = sin(dlat / 2.0) * sin(dlat / 2.0) +
             cos(lat1 * rad) * cos(lat2 * rad) *
             sin(dlon / 2.0) * sin(dlon / 2.0);
  if (a < 0.0) a = 0.0;
  if (a > 1.0) a = 1.0;
  return 6371000.0 * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}
