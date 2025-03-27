#pragma once

#include <QString>

/**
 * @brief Namespace containing map-related content and utilities
 */
namespace MapContent {

/**
 * @brief Get the HTML content for the Mapbox map
 * @param mapboxToken The Mapbox API token
 * @return The HTML content as a QString
 */
QString getMapHtml(const QString& mapboxToken);

} // namespace MapContent
