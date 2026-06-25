(ソース as table) as table => let
    Divided =
        Table.TransformColumns(
            ソース,
            List.Transform(
                List.RemoveItems(Table.ColumnNames(ソース), {"総人口"}),
                (col) => {col, each _ / [([総人口] + 1e-8)], type number}
            )
        )
in
    Divided