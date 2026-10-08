#include "IUI.h"
#include "../Utilities/ILogger.h"
#include <algorithm>

namespace APLG {



/**
 * @brief Get global UI manager instance
 */
static UIManager* g_uiManager = nullptr;

IUIManager* getUIManager() {
    if (!g_uiManager) {
        g_uiManager = new UIManager();
    }
    return g_uiManager;
}

/**
 * @brief Cleanup UI manager
 */
void cleanupUIManager() {
    if (g_uiManager) {
        delete g_uiManager;
        g_uiManager = nullptr;
    }
}

} // namespace APLG
