let
    変換された列 =
        let
            ave = List.Average(削除された他の列[平均給料月額])
        in
            Table.TransformColumns(削除された他の列, {"平均給料月額", each _ - ave, type number}),
    名前が変更された列 = Table.RenameColumns(変換された列, {{"平均給料月額", "偏差"}}),
    変換された列2 =
        let
            sigma = Number.Sqrt(List.Average(List.Transform(名前が変更された列[偏差], each _ * _)))
        in
            Table.TransformColumns(名前が変更された列, {"偏差", each _ / sigma, type number}),
    #"名前が変更された列 1" = Table.RenameColumns(変換された列2, {{"偏差", "k_sigma"}}),
    #"フィルター処理された行 1" = Table.SelectRows(#"名前が変更された列 1", each [k_sigma] <= -3 or 3 <= [k_sigma])
in
    #"フィルター処理された行 1"
