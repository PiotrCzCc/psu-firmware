/*
 * EEZ PSU Firmware
 * Copyright (C) 2015-present, Envox d.o.o.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "psu.h"
#include "scpi_psu.h"
#include "scpi_cal.h"

#include "calibration.h"

namespace eez {
namespace psu {
namespace scpi {

////////////////////////////////////////////////////////////////////////////////

static void select_channel(scpi_t * context, uint8_t ch) {
    scpi_psu_t *psu_context = (scpi_psu_t *)context->user_context;
    psu_context->selected_channel_index = ch;
}

////////////////////////////////////////////////////////////////////////////////

scpi_result_t scpi_inst_Select(scpi_t * context) {
    if (calibration::isEnabled()) {
        SCPI_ErrorPush(context, SCPI_ERROR_BAD_SEQUENCE_OF_CALIBRATION_COMMANDS);
        return SCPI_RES_ERR;
    }

    Channel *channel = param_channel(context, TRUE);
    if (!channel) {
        return SCPI_RES_ERR;
    }

    select_channel(context, channel->index);

    return SCPI_RES_OK;
}

scpi_result_t scpi_inst_SelectQ(scpi_t * context) {
    scpi_psu_t *psu_context = (scpi_psu_t *)context->user_context;

    char buffer[256] = { 0 };
    sprintf_P(buffer, PSTR("CH%d"), (int)psu_context->selected_channel_index);
    SCPI_ResultCharacters(context, buffer, strlen(buffer));

    return SCPI_RES_OK;
}

scpi_result_t scpi_inst_NSelect(scpi_t * context) {
    if (calibration::isEnabled()) {
        SCPI_ErrorPush(context, SCPI_ERROR_BAD_SEQUENCE_OF_CALIBRATION_COMMANDS);
        return SCPI_RES_ERR;
    }

    int32_t ch;
    if (!SCPI_ParamInt(context, &ch, TRUE)) {
        return SCPI_RES_ERR;
    }

    if (!check_channel(context, ch)) {
        return SCPI_RES_ERR;
    }

    select_channel(context, ch);

    return SCPI_RES_OK;
}

scpi_result_t scpi_inst_NSelectQ(scpi_t * context) {
    scpi_psu_t *psu_context = (scpi_psu_t *)context->user_context;

    SCPI_ResultInt(context, psu_context->selected_channel_index);

    return SCPI_RES_OK;
}

}
}
} // namespace eez::psu::scpi