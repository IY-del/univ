let
    Divided =
        Table.TransformColumns(
            ソース,
            List.Transform(
                List.RemoveItems(Table.ColumnNames(ソース), {"X"}),
                each {_, each _ / [X], type number}
            )
        )
in
    Divided