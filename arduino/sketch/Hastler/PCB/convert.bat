@echo off
echo PCB��������Fritzing�t�@�C�����ϊ��o�b�`
echo seeed(Fusion)�̔����ɂ͎g�p�ł���
echo https://www.fusionpcb.jp/index.html
echo switch-science�̔����ɂ͎g�p�ł��Ȃ�����
echo https://www.switch-science.com/
if "%1"=="" goto usage
echo (�� %1�ɂ͔��p�p���L����p���邱��)
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
echo (�� �����ɂ͔��p�p���L����p���邱��)
echo Error �������w�肳��Ă��܂���

:end
pause
