@echo off
echo PCB注文向けFritzingファイル名変換バッチ
echo seeed(Fusion)の発注には使用できた
echo https://www.fusionpcb.jp/index.html
echo switch-scienceの発注には使用できなかった
echo https://www.switch-science.com/
if "%1"=="" goto usage
echo (注 %1には半角英数記号を用いること)
move %1_copperTop.gtl %1.gtl
move %1_copperBottom.gbl %1.gbl
move %1_maskTop.gts %1.gts
move %1_maskBottom.gbs %1.gbs
move %1_silkTop.gto %1.gto
move %1_silkBottom.gbo %1.gbo
move %1_drill.txt %1.txt
move %1_contour.gm1 %1.gml
goto end

:usage
echo (注 引数には半角英数記号を用いること)
echo Error 引数が指定されていません

:end
pause
