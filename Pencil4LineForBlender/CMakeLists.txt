# CMakeList.txt : Pencil4LineForBlender の CMake プロジェクト。ソースを含めて、次を定義します:
# プロジェクト専用ロジックはこちらです。
#


# ソースをこのプロジェクトの実行可能ファイルに追加します。
add_library (Pencil4LineForBlender SHARED
  "blrnaCamera.cpp"
  "blrnaAttribute.cpp"
  "blrnaID.cpp"
  "blrnaIDOverrideLibrary.cpp"
  "blrnaImage.cpp"
  "blrnaMaterial.cpp"
  "blrnaMEdge.cpp"
  "blrnaMesh.cpp"
  "blrnaMeshLoopColorLayer.cpp"
  "blrnaMeshUVLoopLayer.cpp"
  "blrnaMLoop.cpp"
  "blrnaMLoopTri.cpp"
  "blrnaMPoly.cpp"
  "blrnaObject.cpp"
  "intermBlenderVersion.cpp"
  "intermCamera.cpp"
  "intermContext.cpp"
  "intermDataHash.cpp"
  "intermImageMapper.cpp"
  "intermMeshDataAccessor.cpp"
  "intermNameIDMapper.cpp"
  "intermNodeParamsFixer.cpp"
  "intermObjectIDMapper.cpp"
  "Nodes.cpp"
  "Pencil4LineForBlender.cpp"
  "RenderAppSession.cpp"
  "$<IF:$<BOOL:${WIN32}>,FileMapping.cpp,../pencil4line_for_blender_mac/pencil4line_for_blender_mac/FileMappingPOSIX.cpp>"
  "$<IF:$<BOOL:${WIN32}>,RenderAppSessionWindows.cpp,../pencil4line_for_blender_mac/pencil4line_for_blender_mac/RenderAppSessionPOSIX.cpp>"
  # ヘッダーファイルはここに記述しなくても良いが、記述しておくとVisualStudioのソリューションエクスポーラに表示されて見通しが良くなるので、記述しておく。
  "blrnaCamera.h"
  "blrnaAttribute.h"
  "blrnaID.h"
  "blrnaIDOverrideLibrary.h"
  "blrnaImage.h"
  "blrnaMaterial.h"
  "blrnaMEdge.h"
  "blrnaMesh.h"
  "blrnaMeshLoopColorLayer.h"
  "blrnaMeshUVLoopLayer.h"
  "blrnaMLoop.h"
  "blrnaMLoopTri.h"
  "blrnaMPoly.h"
  "blrnaObject.h"
  "commitHash.h"
  "intermBlenderVersion.h"
  "intermCamera.h"
  "intermDataHash.h"
  "intermDrawOptions.h"
  "intermImageMapper.h"
  "intermMeshDataAccessor.h"
  "intermNameIDMapper.h"
  "intermNodeParamsFixer.h"
  "intermObjectIDMapper.h"
  "intermRenderInstance.h"
  "blrnaBase.h"
  "intermContext.h"
  "Nodes.h"
  "RenderAppData.h"
  "RenderAppDefine.h"
  "RenderAppSession.h"
  "StructDefinitions.h"
  "$<IF:$<BOOL:${WIN32}>,FileMapping.h,../pencil4line_for_blender_mac/pencil4line_for_blender_mac/FileMappingPOSIX.hpp>"
  "$<IF:$<BOOL:${WIN32}>,RenderAppSessionWindows.h,../pencil4line_for_blender_mac/pencil4line_for_blender_mac/RenderAppSessionPOSIX.hpp>"
)


if (${WIN32})
  add_custom_command(OUTPUT commitHash.h
    COMMAND makeCommitHash.bat
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
  # ビルド前の処理
  add_custom_command(TARGET Pencil4LineForBlender PRE_BUILD
    COMMAND makeCommitHash.bat
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
else()
  file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/existingCommitHash.sh" CONTENT
    "
    if [ ! -e '${CMAKE_CURRENT_SOURCE_DIR}/commitHash.h' ]; then
        echo
        echo '############################################################################################'
        echo commitHash.hが存在しません
        echo 先に、Windows側でmakeCommitHash.batを実行してCommitHashファイルを生成してください。
        echo ※Linux版はリモートビルドされるため、Gitリポジトリにアクセスできないため
        echo '############################################################################################'
        echo
        exit 1
    fi;
    "
  )
  add_custom_command(OUTPUT commitHash.h
    COMMAND bash ${CMAKE_CURRENT_BINARY_DIR}/existingCommitHash.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()


# よく使う値を変数に保存
set(PythonVer $<IF:$<CONFIG:Debug_Python39,Release_Python39>,39,$<IF:$<CONFIG:Debug_Python310,Release_Python310>,310,311>>) # Pythonバージョン（ドット無し）
set(PythonVerPoint $<IF:$<CONFIG:Debug_Python39,Release_Python39>,3.9,$<IF:$<CONFIG:Debug_Python310,Release_Python310>,3.10,3.11>>) # Pythonバージョン（ドット無し）
set(IsDebug $<CONFIG:Debug_Python39,Debug_Python310,Debug_Python311>) # デバッグ構成かどうか　（TODO:プリセットの方で定義した方が取り回しが楽かもしれない）


# 全般オプション
set_property(TARGET Pencil4LineForBlender PROPERTY DEBUG_CONFIGURATIONS Debug_Python39,Debug_Python310) # どれがデバッグ構成なのかを指定
set_target_properties(Pencil4LineForBlender PROPERTIES
  CXX_STANDARD 17
  OUTPUT_NAME pencil4line_for_blender_${OsName}_${PythonVer}
  PREFIX "" # デフォルトで「lib」が入っているので空にする
  SUFFIX ${Suffix}
  INTERPROCEDURAL_OPTIMIZATION_RELEASE_PYTHON39 TRUE # プログラム全体の最適化
  INTERPROCEDURAL_OPTIMIZATION_RELEASE_PYTHON310 TRUE
  INTERPROCEDURAL_OPTIMIZATION_RELEASE_PYTHON311 TRUE
)


# コンパイルオプション
target_compile_definitions(Pencil4LineForBlender PRIVATE
  UNICODE _UNICODE
  $<$<BOOL:${WIN32}>:WIN32 _WINDOWS $<${IsDebug}:_DEBUG>>
  $<$<AND:$<NOT:$<BOOL:${WIN32}>>,${IsDebug}>:DEBUG=1>
  PYTHON${PythonVer}
  PYBIND_INCLUDE PENCIL4LINEFORBLENDER_EXPORTS _USRDLL
)
target_include_directories(Pencil4LineForBlender PRIVATE
  ${CMAKE_SOURCE_DIR}/pybind11/include
  ${CMAKE_SOURCE_DIR}/cereal/include
  $<$<BOOL:${WIN32}>:$ENV{LOCALAPPDATA}/Programs/Python/Python${PythonVer}/include>
  $<$<NOT:$<BOOL:${WIN32}>>:/usr/include/python${PythonVerPoint}>
  ${CMAKE_SOURCE_DIR}/blender/makesdna
  ${CMAKE_SOURCE_DIR}/blender/makesrna
  ${CMAKE_SOURCE_DIR}/blender/python
  ${CMAKE_SOURCE_DIR}/Pencil4LineForBlender
)
if (${WIN32})
  target_compile_options(Pencil4LineForBlender PRIVATE
    /sdl /permissive- # セキュリティ警告有効 C++標準準拠モード有効
    $<IF:${IsDebug},/MDd /Od,/Gy /Oi /O2> # デバッグ時：デバッグDLLCRT ｜ リリース時：関数レベルリンク、組込関数の使用、最適化レベル2
  )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  target_compile_options(Pencil4LineForBlender PRIVATE
    -maes -mssse3 -fPIC -fvisibility-inlines-hidden -ffast-math
    $<IF:${IsDebug},-g2,-funroll-loops -O2>
    -fdata-sections -ffunction-sections # 未使用コードのストリップのため
    # 以下は警告の有効化（デフォルトで有効な警告はここには記述していない）
    -Wcomma -Wdocumentation -Winfinite-recursion -Wrange-loop-analysis -Wmove -Wduplicate-method-match
  )
endif()


# リンカオプション
# 新しい構成名を作ったときは以下の変数をセットしないとCMAKEが警告する（プロパティで同等の設定をしてもダメ）ので、セット
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG_PYTHON39 ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG_PYTHON310 ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG_PYTHON311 ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_PYTHON39 ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_PYTHON310 ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_PYTHON311 ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
if (${WIN32})
  target_link_directories(Pencil4LineForBlender PRIVATE $ENV{LOCALAPPDATA}/Programs/Python/Python${PythonVer}/libs)
  target_link_options(Pencil4LineForBlender PRIVATE
    /SUBSYSTEM:WINDOWS
    /DEBUG # デバッグ/リリースに関わらずデバッグ情報を出力
    $<$<NOT:${IsDebug}>:/OPT:ICF /OPT:REF> # リリース時： COMDAT圧縮 参照されない関数/データの削除
  )
  target_link_libraries(Pencil4LineForBlender PRIVATE
    python3.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    comdlg32.lib
    advapi32.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    odbc32.lib
    odbccp32.lib
  )
else()
  target_link_options(Pencil4LineForBlender PRIVATE
    LINKER:--gc-sections # 未使用コードのストリップ
  )
  target_link_libraries(Pencil4LineForBlender PRIVATE
    librt.so 
  )
# TODO:デフォルトのリンカはldなのでより高速なlldやmoldに替える？
endif()


# ビルド後の処理
# ビルド結果をPackageフォルダにコピー
# CMAKEコマンドラインのコピー機能を使うことによって各プラットフォームで動作する
add_custom_command(TARGET Pencil4LineForBlender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Pencil4LineForBlender> ${CMAKE_SOURCE_DIR}/Package/bin
)

message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM=${CMAKE_HOST_SYSTEM}")
message("CMAKE_HOST_WIN32=${CMAKE_HOST_WIN32}")
message("CMAKE_HOST_LINUX=${CMAKE_HOST_LINUX}")
message("CMAKE_HOST_UNIX=${CMAKE_HOST_UNIX}")
message("WIN32=${WIN32}")
message("LINUX=${LINUX}")
message("UNIX=${UNIX}")


# TODO: VisualStudioソリューションエクスプローラ上での論理フォルダ構成を設定（ファイルシステム上とは別のフォルダ構成を作れる）
# set_target_properties(MyLibrary PROPERTIES FOLDER "MyFolder")

# TODO: テストを追加し、必要な場合は、ターゲットをインストールします。


# 以下はデバッグ用のコマンド等

# 変数やプロパティの内容を出力
#include(CMakePrintHelpers)
#cmake_print_properties(
#    TARGETS Pencil4LineForBlender
#    PROPERTIES LINK_FLAGS LINK_OPTIONS
#)

# ジェネレータエクスプレッションの評価結果を確認したいときはファイルに出力する
#file(GENERATE OUTPUT ${CMAKE_SOURCE_DIR}/GexprTest_$<CONFIG>.txt CONTENT "ここに確認したいジェネレータエクスプレッション")
