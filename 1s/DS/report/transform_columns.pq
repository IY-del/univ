(ソース as table) as table => let
    追加された人口あたりの列 = let
            baseColumns = List.RemoveItems(Table.ColumnNames(ソース), {"総人口"}),
            addOneColumn =
                (state as table, col as text) as table =>
                    Table.AddColumn(
                        state,
                        "人口あたり" & col,
                        each Record.Field(_, col) / (Record.Field(_, "総人口") + 1e-8),
                        type number
                    )
    in List.Accumulate(baseColumns, ソース, addOneColumn)
in 追加された人口あたりの列