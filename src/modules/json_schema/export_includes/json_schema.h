#pragma once

#include <rgen/common/link_error_helper.h>

#include <QByteArray>

/*!
 * \brief Redkit-gen модуль JsonSchema
 * \details Модуль
 * \warning Не для вызова, а для поддержки переходов по символу в Doxygen
 */
void JsonSchema();

namespace rgen
{

template <class S, class LinkErrorHelper = RedkitGenLinkErrorHelper>
class JsonSchema
{
public:
    /*!
     * \brief получить схему
     */
    static std::string getJsonSchema();
};

} // namespace rgen
