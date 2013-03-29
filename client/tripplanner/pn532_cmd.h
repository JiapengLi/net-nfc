#ifndef PN532_CMD_H
#define PN532_CMD_H

// Miscellaneous
#define CmdDiagnose 0x00
#define CmdGetFirmwareVersion 0x02
#define CmdGetGeneralStatus 0x04
#define CmdReadRegister 0x06
#define CmdWriteRegister 0x08
#define CmdReadGPIO 0x0C
#define CmdWriteGPIO 0x0E
#define CmdSetSerialBaudRate 0x10
#define CmdSetParameters 0x12
#define CmdSAMConfiguration 0x14
#define CmdPowerDown 0x16
#define CmdAlparCommandForTDA 0x18
// RC-S360 has another command 0x18 for reset &..?

// RF communication
#define CmdRFConfiguration 0x32
#define CmdRFRegulationTest 0x58

// Initiator
#define CmdInJumpForDEP 0x56
#define CmdInJumpForPSL 0x46
#define CmdInListPassiveTarget 0x4A
#define CmdInATR 0x50
#define CmdInPSL 0x4E
#define CmdInDataExchange 0x40
#define CmdInCommunicateThru 0x42
#define CmdInQuartetByteExchange 0x38
#define CmdInDeselect 0x44
#define CmdInRelease 0x52
#define CmdInSelect 0x54
#define CmdInActivateDeactivatePaypass 0x48
#define CmdInAutoPoll 0x60

// Target
#define CmdTgInitAsTarget 0x8C
#define CmdTgSetGeneralBytes 0x92
#define CmdTgGetData 0x86
#define CmdTgSetData 0x8E
#define CmdTgSetDataSecure 0x96
#define CmdTgSetMetaData 0x94
#define CmdTgSetMetaDataSecure 0x98
#define CmdTgGetInitiatorCommand 0x88
#define CmdTgResponseToInitiator 0x90
#define CmdTgGetTargetStatus 0x8A

/** @note PN53x's normal frame:
 *
 *   .-- Start
 *   |   .-- Packet lenght
 *   |   |  .-- Lenght checksum
 *   |   |  |  .-- Direction (D4 Host to PN, D5 PN to Host)
 *   |   |  |  |  .-- Code
 *   |   |  |  |  |  .-- Packet checksum
 *   |   |  |  |  |  |  .-- Postamble
 *   V   |  |  |  |  |  |
 * ----- V  V  V  V  V  V
 * 00 FF 02 FE D4 02 2A 00
 */

/** @note PN53x's extended frame:
 *
 *   .-- Start
 *   |     .-- Fixed to FF to enable extended frame
 *   |     |     .-- Packet lenght
 *   |     |     |   .-- Lenght checksum
 *   |     |     |   |  .-- Direction (D4 Host to PN, D5 PN to Host)
 *   |     |     |   |  |  .-- Code
 *   |     |     |   |  |  |  .-- Packet checksum
 *   |     |     |   |  |  |  |  .-- Postamble
 *   V     V     V   |  |  |  |  |
 * ----- ----- ----- V  V  V  V  V
 * 00 FF FF FF 00 02 FE D4 02 2A 00
 */

#endif // PN532_CMD_H
