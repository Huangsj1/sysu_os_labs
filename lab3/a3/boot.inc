;常量定义区
;___________________Loader__________________
;加载器扇区数量
LOADER_SECTOR_COUNT equ 5
;加载器起始扇区
LOADER_START_SECTOR equ 1
;加载器被加载的地址
LOADER_START_ADDRESS equ 0x7e00

;__________________GDT______________________
;GDT起始位置
GDT_START_ADDRESS equ 0x8800

;__________________段选择子________________
;数据段选择子
DATA_SELECTOR equ 0x8
;栈段选择子
STACK_SELECTOR equ 0x10
;视频段选择子
VIDEO_SELECTOR equ 0x18
VIDEO_NUM equ 0x18
;代码段选择子
CODE_SELECTOR equ 0x20
