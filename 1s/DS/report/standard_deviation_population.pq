(x as list) as number => let
  m = List.Average(x),
  std = Number.Sqrt(List.Average(List.Transform(x, each Number.Power((_ - m), 2))))
in std