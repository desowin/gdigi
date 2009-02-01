/*
 *  Copyright (c) 2009 Tomasz Moń <desowin@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses>.
 */

#include "gdigi.h"

void test_wah(struct usb_dev_handle *handle);
void test_compressor(struct usb_dev_handle *handle);
void test_dist(struct usb_dev_handle *handle);
void test_presets(struct usb_dev_handle *handle);
void test_pickups(struct usb_dev_handle *handle);
void test_eq(struct usb_dev_handle *handle);
void test_noisegate(struct usb_dev_handle *handle);
void test_chorusfx(struct usb_dev_handle *handle);
void test_delay(struct usb_dev_handle *handle);
void test_reverb(struct usb_dev_handle *handle);
void test_all(struct usb_dev_handle *handle);
