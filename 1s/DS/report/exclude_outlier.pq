(ソース as table) as table =>
let
    ave = List.Average(ソース[人口密度]),
    sigma = standard_deviation_population(ソース[人口密度]),
    追加された列 =
        Table.AddColumn(
            ソース,
            "k_sigma",
            each ([人口密度] - ave) / sigma,
            type number
        ),
    フィルター処理された行 = Table.SelectRows(追加された列, each Number.Abs([k_sigma]) <= 3
)
in
    フィルター処理された行
