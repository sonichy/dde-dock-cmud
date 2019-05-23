cmake_minimum_required(VERSION 3.9.5)

# 使用 set 命令设置一个变量
set(PLUGIN_NAME "HTYCMDU")

# 设置项目名称
project(${PLUGIN_NAME})

# 启用 qt moc 的支持
set(CMAKE_AUTOMOC ON)
# 启用 qrc 资源文件的支持
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRCS "*.h" "*.cpp")


find_package(Qt5Widgets REQUIRED)
# sudo apt install libdtkwidget-dev
find_package(DtkWidget REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(DdeDockInterface REQUIRED dde-dock)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")

add_library(${PLUGIN_NAME} SHARED ${SRCS} res.qrc)

set_target_properties(${PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ./)

target_include_directories(${PLUGIN_NAME} PUBLIC
	    ${Qt5Widgets_INCLUDE_DIRS}
	        ${DtkWidget_INCLUDE_DIRS}
		    ${DdeDockInterface_INCLUDE_DIRS}
		    )

	    target_link_libraries(${PLUGIN_NAME} PRIVATE
		        ${Qt5Widgets_LIBRARIES}
			    ${DtkWidget_LIBRARIES}
			        ${DdeDockInterface_LIBRARIES}
				)

			# 设置安装路径的前缀(默认为"/usr/local")
			set(CMAKE_INSTALL_PREFIX "/usr")

			# 设置执行 make install 时哪个目标应该被 install 到哪个位置
			install(TARGETS ${PLUGIN_NAME} LIBRARY DESTINATION lib/dde-dock/plugins)
