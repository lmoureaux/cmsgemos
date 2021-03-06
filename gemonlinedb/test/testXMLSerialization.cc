#include <memory>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include "gem/onlinedb/VFAT3ChipConfiguration.h"
#include "gem/onlinedb/XMLSerializationData.h"
#include "gem/onlinedb/detail/XMLUtils.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE XMLSerialization
#include <boost/test/unit_test.hpp>

/* Needed to make the linker happy. */
#include <xdaq/version.h>
config::PackageInfo xdaq::getPackageInfo()
{
    return config::PackageInfo("", "", "", "", "", "", "", "");
}

using namespace gem::onlinedb;
using namespace gem::onlinedb::detail::literals;

BOOST_FIXTURE_TEST_SUITE(XMLSerialization, detail::XercesGuard)

XMLSerializationData<VFAT3ChipConfiguration> createTestXMLSerializationData()
{
    XMLSerializationData<VFAT3ChipConfiguration> builder;

    Run r;
    r.location = "Brussels";
    r.initiatingUser = "lmoureau";
    builder.setRun(r);

    DataSet<VFAT3ChipConfiguration> ds;
    ds.setComment("Test data generated by cmsgemos");
    ds.setVersion("pre-alpha");
    ds.setPart(PartReferenceBarcode{ "0/0" });
    ds.addData(VFAT3ChipConfiguration());
    ds.addData(VFAT3ChipConfiguration());
    builder.addDataSet(ds);

    return builder;
}

BOOST_AUTO_TEST_CASE(MakeDOM)
{
    XERCES_CPP_NAMESPACE_USE

    auto builder = createTestXMLSerializationData();
    auto dom = builder.makeDOM();

    BOOST_CHECK(dom != nullptr);

    auto root = dom->getDocumentElement();

    BOOST_CHECK(root != nullptr);
    BOOST_CHECK(root->getChildNodes()->getLength() == 2);

    BOOST_CHECK(detail::transcode(root->getFirstChild()->getNodeName()) ==
                std::string("HEADER"));
    BOOST_CHECK(detail::transcode(root->getLastChild()->getNodeName()) ==
                std::string("DATA_SET"));
}

BOOST_AUTO_TEST_CASE(MakeDOMXsdValidation)
{
    XERCES_CPP_NAMESPACE_USE

    auto builder = createTestXMLSerializationData();
    auto dom = builder.makeDOM();

    try {
        // In principle, DOM 3 has an API to validate documents. One should
        // modify the configuration of the document (getDOMConfig) and call
        // normalizeDocument().
        // However, the DOMImplementation available in Xerces doesn't implement
        // this particular feature (an exception is thrown when trying to use
        // it). Documents can, however, be validated at load time.
        // So below we serialize the document to a memory buffer and validate it
        // by reading it back.

        // Get an implementation
        auto impl = DOMImplementationRegistry::getDOMImplementation("LS"_xml);
        BOOST_REQUIRE(impl != nullptr);

        auto implLS = dynamic_cast<DOMImplementationLS *>(impl);

        // Create the serializer, write and delete it
        auto lsser = std::unique_ptr<DOMLSSerializer>();
        lsser.reset(implLS->createLSSerializer());
        BOOST_REQUIRE(lsser != nullptr);

        // Serialize
        auto buffer = lsser->writeToString(dom->getDocumentElement());
        BOOST_REQUIRE(buffer != nullptr);
        BOOST_REQUIRE(XMLString::stringLen(buffer) > 0);

        // Create an input source and a parser
        MemBufInputSource source(
            reinterpret_cast<XMLByte *>(buffer),
            XMLString::stringLen(buffer) * 2, // * 2 because of UTF-16 encoding
            "fake"_xml, // Doc says it's used as a "fake system id"
            true); // The buffer will be freed automatically
        source.setEncoding("UTF-16"_xml);

        auto errorHandler = std::make_shared<detail::XercesAlwaysThrowErrorHandler>();

        XercesDOMParser parser;
        parser.setErrorHandler(errorHandler.get());
        parser.setExternalNoNamespaceSchemaLocation("xml/schema/VFAT3ChipConfiguration.xsd");
        parser.setValidationScheme(XercesDOMParser::Val_Always);
        parser.setDoNamespaces(true);
        parser.setDoSchema(true);
        parser.setValidationSchemaFullChecking(true);

        // Validate
        parser.parse(source);

    } catch (DOMException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (XMLException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (SAXParseException &e) {
        auto column = e.getColumnNumber();
        auto line = e.getLineNumber();
        throw std::runtime_error(
            std::to_string(line) + ":" +
            std::to_string(column) + ": " +
            detail::transcode(e.getMessage()));
    } catch (SAXException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    }
}

BOOST_AUTO_TEST_CASE(MakeDOMReadDOM)
{
    XERCES_CPP_NAMESPACE_USE

    auto data = createTestXMLSerializationData();
    auto dom = data.makeDOM();

    try {

        auto data2 = XMLSerializationData<VFAT3ChipConfiguration>();
        data2.readDOM(dom);

        BOOST_REQUIRE(data.getRun() == data2.getRun());
        BOOST_REQUIRE(data.getDataSets() == data2.getDataSets());

    } catch (DOMException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (XMLException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    } catch (SAXParseException &e) {
        auto column = e.getColumnNumber();
        auto line = e.getLineNumber();
        throw std::runtime_error(
            std::to_string(line) + ":" +
            std::to_string(column) + ": " +
            detail::transcode(e.getMessage()));
    } catch (SAXException &e) {
        throw std::runtime_error(detail::transcode(e.getMessage()));
    }
}

BOOST_AUTO_TEST_SUITE_END()
