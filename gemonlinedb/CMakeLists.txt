#
# Run parseDef.py to generate configuration classes
#
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/gem/onlinedb/detail)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/common)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/xml/schema)

function(run_parseDef file)
    add_custom_command(OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/include/gem/onlinedb/detail/${file}Gen.h
        ${CMAKE_CURRENT_BINARY_DIR}/src/common/${file}Gen.cc
        ${CMAKE_CURRENT_BINARY_DIR}/xml/schema/${file}.xsd
        COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/parseDef.py
            ${CMAKE_CURRENT_LIST_DIR}/defs/${file}.json
        DEPENDS parseDef.py defs/${file}.json
        COMMENT "Running parseDef.py: ${file}"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xml/schema/${file}.xsd
            DESTINATION share/gemonlinedb/xml/schema/
            COMPONENT gemonlinedb)
endfunction(run_parseDef)

run_parseDef("AMCConfiguration")
run_parseDef("OHv3Configuration")
run_parseDef("VFAT3ChipConfiguration")

#
# Main target
#
add_cmsgemos_module(gemonlinedb SOURCES
    AMC13Configuration.cc
    AMCConfiguration.cc
    ConfigurationLinker.cc
    DBInterface.cc
    FileUtils.cc
    GEMOnlineDBManager.cc
    GEMOnlineDBManagerWeb.cc
    OHv3Configuration.cc
    Run.cc
    SystemTopology.cc
    version.cc
    VFAT3ChannelConfiguration.cc
    XMLConfigurationProvider.cc
    XMLSerializationData.cc
    XMLUtils.cc

    GENERATED_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/AMCConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/OHv3ConfigurationGen.cc
    ${CMAKE_CURRENT_BINARY_DIR}/src/common/VFAT3ChipConfigurationGen.cc
)
target_link_libraries(gemonlinedb gembase)
target_link_libraries(gemonlinedb xdaq)

install(DIRECTORY html DESTINATION /opt/xdaq/htdocs/cmsgemos/gemonlinedb COMPONENT gemonlinedb)
install(DIRECTORY xml DESTINATION share/gemonlinedb COMPONENT gemonlinedb)
