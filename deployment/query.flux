-- example flux query for use in Grafana.

from(bucket: "ParkSenseBucket")
  |> range(start: -24h)
  |> filter(fn: (r) => r._field == "occupied")
  |> aggregateWindow(every: 1m, fn: last, createEmpty: false)
  |> group(columns: ["_time"])
  |> sum()
  |> group()