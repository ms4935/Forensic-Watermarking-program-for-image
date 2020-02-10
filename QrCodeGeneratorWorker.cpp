
#include "stdafx.h"


#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include "QrCode.hpp"

using qrcodegen::QrCode;
using qrcodegen::QrSegment;


static const std::vector<QrCode::Ecc> ECC_LEVELS{
	QrCode::Ecc::LOW,
	QrCode::Ecc::MEDIUM,
	QrCode::Ecc::QUARTILE,
	QrCode::Ecc::HIGH,
};


int mai() {
	while (true) {

		// Read data length or exit
		int length;
		std::cin >> length;
		if (length == -1)
			break;

		// Read data bytes
		bool isAscii = true;
		std::vector<uint8_t> data;
		for (int i = 0; i < length; i++) {
			int b;
			std::cin >> b;
			data.push_back(static_cast<uint8_t>(b));
			isAscii &= 0 < b && b < 128;
		}

		// Read encoding parameters
		int errCorLvl, minVersion, maxVersion, mask, boostEcl;
		std::cin >> errCorLvl;
		std::cin >> minVersion;
		std::cin >> maxVersion;
		std::cin >> mask;
		std::cin >> boostEcl;

		// Make list of segments
		std::vector<QrSegment> segs;
		if (isAscii) {
			std::vector<char> text(data.cbegin(), data.cend());
			text.push_back('\0');
			segs = QrSegment::makeSegments(text.data());
		}
		else
			segs.push_back(QrSegment::makeBytes(data));

		try {  // Try to make QR Code symbol
			const QrCode qr = QrCode::encodeSegments(segs,
				ECC_LEVELS.at(errCorLvl), minVersion, maxVersion, mask, boostEcl == 1);
			// Print grid of modules
			std::cout << qr.getVersion() << std::endl;
			for (int y = 0; y < qr.getSize(); y++) {
				for (int x = 0; x < qr.getSize(); x++)
					std::cout << (qr.getModule(x, y) ? 1 : 0) << std::endl;
			}

		}
		catch (const qrcodegen::data_too_long &ex) {
			std::cout << -1 << std::endl;
		}
		std::cout << std::flush;
	}
	return EXIT_SUCCESS;
}
