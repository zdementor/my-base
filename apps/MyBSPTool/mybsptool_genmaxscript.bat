set src_fname=robo_hangar_part1
set dst_fname=robo_hangar_part1

set src_path=..\robo.media.work\baseq3\maps\
set dst_path=..\robo.media\models\maps\

set light_intens_mul=0.5
set light_radius_mul=1.0

call mybsptool.exe -rempatch %src_path%%src_fname%.bsp %src_path%%src_fname%_rempatches.bsp
call mybsptool.exe -collapse %src_path%%src_fname%_rempatches.bsp %src_path%%src_fname%_colapsed.bsp

copy %src_path%%src_fname%.map %src_path%%src_fname%_colapsed.map

call mybsptool.exe -genmaxscr   %src_path%%src_fname%_colapsed.bsp %src_path%%src_fname%_colapsed.ms %light_intens_mul% %light_radius_mul%


rem del  %src_path%..\lightmaps\*.* /q
mkdir %src_path%..\lightmaps\
echo call mybsptool.exe -writelmap %src_path%%src_fname%.bsp %src_path%%src_fname%_lightmapped.bsp >mybsptool_writelightmaps.bat
echo copy %src_path%%src_fname%_lightmapped.bsp %dst_path%%dst_fname%.bsp >>mybsptool_writelightmaps.bat




