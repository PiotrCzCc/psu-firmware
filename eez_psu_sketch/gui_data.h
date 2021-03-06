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

#pragma once

#include "event_queue.h"

namespace eez {
namespace psu {
namespace gui {
namespace data {

////////////////////////////////////////////////////////////////////////////////

enum ValueType {
    VALUE_TYPE_NONE,
    VALUE_TYPE_INT,
	VALUE_TYPE_FLOAT,
    VALUE_TYPE_FLOAT_VOLT,
    VALUE_TYPE_FLOAT_AMPER,
    VALUE_TYPE_FLOAT_MILLI_VOLT,
    VALUE_TYPE_FLOAT_MILLI_AMPER,
	VALUE_TYPE_FLOAT_WATT,
	VALUE_TYPE_FLOAT_SECOND,
	VALUE_TYPE_FLOAT_CELSIUS,
	VALUE_TYPE_FLOAT_RPM,
    VALUE_TYPE_CONST_STR,
    VALUE_TYPE_STR,
    VALUE_TYPE_CHANNEL_LABEL,
	VALUE_TYPE_CHANNEL_SHORT_LABEL,
	VALUE_TYPE_CHANNEL_BOARD_INFO_LABEL,
	VALUE_TYPE_LESS_THEN_MIN_FLOAT,
	VALUE_TYPE_LESS_THEN_MIN_INT,
	VALUE_TYPE_LESS_THEN_MIN_TIME_ZONE,
	VALUE_TYPE_GREATER_THEN_MAX_FLOAT,
	VALUE_TYPE_GREATER_THEN_MAX_INT,
	VALUE_TYPE_GREATER_THEN_MAX_TIME_ZONE,
	VALUE_TYPE_EVENT,
	VALUE_TYPE_PAGE_INFO,
	VALUE_TYPE_ON_TIME_COUNTER,
	VALUE_TYPE_SCPI_ERROR_TEXT,
	VALUE_TYPE_TIME_ZONE,
	VALUE_TYPE_YEAR,
	VALUE_TYPE_MONTH,
	VALUE_TYPE_DAY,
	VALUE_TYPE_HOUR,
	VALUE_TYPE_MINUTE,
	VALUE_TYPE_SECOND,
	VALUE_TYPE_USER_PROFILE_LABEL,
	VALUE_TYPE_USER_PROFILE_REMARK,
    VALUE_TYPE_EDIT_INFO,
    VALUE_TYPE_IP_ADDRESS
};

struct Value {
    Value() : type_(VALUE_TYPE_NONE) { }
    Value(int value) : type_(VALUE_TYPE_INT), int_(value)  {}
	Value(int value, ValueType type) : type_(type), int_(value)  {}
	Value(uint8_t value, ValueType type) : type_(type), uint8_(value)  {}
	Value(uint16_t value, ValueType type) : type_(type), uint16_(value)  {}
	Value(uint32_t value, ValueType type) : type_(type), uint32_(value)  {}
    Value(float value, ValueType type) : type_(type), float_(value) {}
    Value(const char *str) : type_(VALUE_TYPE_STR), str_(str) {}
	Value(event_queue::Event *e) : type_(VALUE_TYPE_EVENT), event_(e) {}

    static Value ProgmemStr(const char *pstr PROGMEM) {
        Value value;
        value.const_str_ = pstr;
        value.type_ = VALUE_TYPE_CONST_STR;
        return value;
    }

	static Value PageInfo(uint8_t pageIndex, uint8_t numPages) {
		Value value;
		value.pageInfo_.pageIndex = pageIndex;
		value.pageInfo_.numPages = numPages;
		value.type_ = VALUE_TYPE_PAGE_INFO;
		return value;
	}

	static Value ScpiErrorText(int16_t errorCode) {
		Value value;
		value.int16_ = errorCode;
		value.type_ = VALUE_TYPE_SCPI_ERROR_TEXT;
		return value;
	}

	static Value LessThenMinMessage(float float_, ValueType type) {
		Value value;
		if (type == VALUE_TYPE_INT) {
			value.int_ = int(float_);
			value.type_ = VALUE_TYPE_LESS_THEN_MIN_INT;
		} else if (type == VALUE_TYPE_TIME_ZONE) {
			value.type_ = VALUE_TYPE_LESS_THEN_MIN_TIME_ZONE;
		} else {
			value.float_ = float_;
			value.type_ = VALUE_TYPE_LESS_THEN_MIN_FLOAT;
		}
		return value;
	}

	static Value GreaterThenMaxMessage(float float_, ValueType type) {
		Value value;
		if (type == VALUE_TYPE_INT) {
			value.int_ = int(float_);
			value.type_ = VALUE_TYPE_GREATER_THEN_MAX_INT;
		} else if (type == VALUE_TYPE_TIME_ZONE) {
			value.type_ = VALUE_TYPE_GREATER_THEN_MAX_TIME_ZONE;
		} else {
			value.float_ = float_;
			value.type_ = VALUE_TYPE_GREATER_THEN_MAX_FLOAT;
		}
		return value;
	}

	bool operator ==(const Value &other) {
        if (type_ != other.type_) {
            return false;
        }

        if (type_ == VALUE_TYPE_NONE || type_ == VALUE_TYPE_LESS_THEN_MIN_TIME_ZONE || type_ == VALUE_TYPE_GREATER_THEN_MAX_TIME_ZONE) {
            return true;
        }
		
		if (type_ == VALUE_TYPE_INT || type_ == VALUE_TYPE_CHANNEL_LABEL || type_ == VALUE_TYPE_CHANNEL_SHORT_LABEL || type_ == VALUE_TYPE_CHANNEL_BOARD_INFO_LABEL || type_ == VALUE_TYPE_LESS_THEN_MIN_INT || type_ == VALUE_TYPE_GREATER_THEN_MAX_INT || type_ == VALUE_TYPE_USER_PROFILE_LABEL || type_ == VALUE_TYPE_USER_PROFILE_REMARK || type_ == VALUE_TYPE_EDIT_INFO) {
            return int_ == other.int_;
        }

		if (type_ == VALUE_TYPE_SCPI_ERROR_TEXT || type_ == VALUE_TYPE_TIME_ZONE) {
			return int16_ == other.int16_;
		}

		if (type_ >= VALUE_TYPE_MONTH && type_ <= VALUE_TYPE_SECOND) {
			return uint8_ == other.uint8_;
		}

		if (type_ == VALUE_TYPE_YEAR) {
			return uint16_ == other.uint16_;
		}

		if (type_ == VALUE_TYPE_ON_TIME_COUNTER || type_ == VALUE_TYPE_IP_ADDRESS) {
			return uint32_ == other.uint32_;
		}
		
		if (type_ == VALUE_TYPE_STR) {
            return strcmp(str_, other.str_) == 0;
        }

		if (type_ == VALUE_TYPE_CONST_STR) {
            return const_str_ == other.const_str_;
        }
		
		if (type_ == VALUE_TYPE_EVENT) {
            return event_->dateTime == other.event_->dateTime && event_->eventId == other.event_->eventId;
        }
		
		if (type_ == VALUE_TYPE_PAGE_INFO) {
			return pageInfo_.pageIndex == other.pageInfo_.pageIndex && pageInfo_.numPages == other.pageInfo_.numPages;
		}
        
		return util::equal(float_, other.float_, CHANNEL_VALUE_PRECISION);
    }

    bool operator !=(const Value &other) {
        return !(*this == other);
    }

    float getFloat() const { return float_; }
    
    ValueType getType() const { return (ValueType)type_; }

    uint8_t getInt() const { return int_; }

    void toText(char *text, int count) const;

	bool isString() { return type_ == VALUE_TYPE_STR; }
	const char *asString() { return str_; }

	uint8_t getPageIndex() { return pageInfo_.pageIndex; }
	uint8_t getNumPages() { return pageInfo_.numPages; }

private:
    uint8_t type_;
    union {
        int int_;
		int16_t int16_;
		uint8_t uint8_;
		uint16_t uint16_;
		uint32_t uint32_;

		float float_;
        
		const char *const_str_ PROGMEM;
        const char *str_;
		
		event_queue::Event *event_;
		
		struct {
			uint8_t pageIndex;
			uint8_t numPages;
		} pageInfo_;
    };
};

////////////////////////////////////////////////////////////////////////////////

struct Cursor {
    int i;

    Cursor() {
        i = -1;
    }

    Cursor(int i) {
        this->i = i;
    }

    operator bool() {
        return i != -1;
    }

    bool operator != (const Cursor& rhs) const {
        return !(*this == rhs);
    }

    bool operator == (const Cursor& rhs) const {
        return i == rhs.i;
    }

	void reset() {
		i = -1;
	}
};

////////////////////////////////////////////////////////////////////////////////

extern Value g_alertMessage;

int count(uint8_t id);
void select(Cursor &cursor, uint8_t id, int index);

Value getMin(const Cursor &cursor, uint8_t id);
Value getMax(const Cursor &cursor, uint8_t id);
ValueType getUnit(const Cursor &cursor, uint8_t id);

void getButtonLabels(const Cursor &cursor, uint8_t id, const Value **labels, int &count);

bool set(const Cursor &cursor, uint8_t id, Value value, int16_t *error);

}
}
}
} // namespace eez::psu::ui::data
