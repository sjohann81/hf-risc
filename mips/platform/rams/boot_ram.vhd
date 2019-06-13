library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
library UNISIM;
use UNISIM.vcomponents.all;

entity ram is
   generic(memory_type : string := "DEFAULT");
   port(clk               : in std_logic;
        enable            : in std_logic;
        write_byte_enable : in std_logic_vector(3 downto 0);
        address           : in std_logic_vector(31 downto 2);
        data_write        : in std_logic_vector(31 downto 0);
        data_read         : out std_logic_vector(31 downto 0));
end; --entity ram

architecture logic of ram is
   constant ZERO          : std_logic_vector(31 downto 0) := "00000000000000000000000000000000";
begin

   RAMB16_S9_inst0 : RAMB16_S9
   generic map (
INIT_00 => X"0202000c0200020c020200260800000000afafafafafaf3caf270010000c273c",
INIT_01 => X"3ca208262703008f8f8f8f8f8f8fa214a00226248280a01002242406a0162690",
INIT_02 => X"038f028f000c0010afaf2c272400038c3c10308c3c30038c3c0003ac3c14308c",
INIT_03 => X"1424000000040010000824042410000003001400001030001027038fa0028f27",
INIT_04 => X"00008f000c00040004afaf272408000300030014001400000000140000001000",
INIT_05 => X"240824240000afafafafafafaf2727038f0000008f3a000c0024040027038f00",
INIT_06 => X"24240c240c2a12001424142c2424142c24142e24108f10000c02102a00260c00",
INIT_07 => X"001480002732260c001083240cafaf272727038f8f8f8f8f8f028f00142a260c",
INIT_08 => X"08240c27038f8f001480002732260c001083240c27020014afaf2c2727038f8f",
INIT_09 => X"0c26243c001482260c26ac34243c8cac3cacafafafafafafafafafaf243c2702",
INIT_0A => X"3c001482260c26243c001482260c26243c3624242726263c0c3c3c243c148226",
INIT_0B => X"102c26000c000c240c001482260c26243c001482260c26243c001482260c2624",
INIT_0C => X"0c26243c3c3c08ae000c001482260c2624003c0c001482260c022400008c0200",
INIT_0D => X"3c0c001482260c02242608241032260c2610000c24120208a002000c00148226",
INIT_0E => X"0c02243c08001626a202301a000c001482260c2624003c0c001482260c262400",
INIT_0F => X"82260c2426020c3c240c020026002418000c001482260c2624003c0c00148226",
INIT_10 => X"0c001482260c02242608240c0016240c900c022624120012240c900c02240014",
INIT_11 => X"8f08260caf00083c0c001482260c02243c080002001482260c26243c3c00083c",
INIT_12 => X"0c26001482260c263c3c080016020c2416260c2400142c249226001482260c03",
INIT_13 => X"6c6864393531333762666a6e72767a00633834304c3c3c08001482260c24263c",
INIT_14 => X"00646d200a2062200a79696175200a30324a2d647420520a202025780a787470",
INIT_15 => X"0a202928670a740a6520002d2e612020740a7820720a746577200a00666d200a",
INIT_16 => X"000000000000000000000000000000000000000000000000000065610a006565",
INIT_17 => X"0100000000000000000000000000000000000000000000000000000000000000",
INIT_18 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_19 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_20 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_21 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_22 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_23 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_24 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_25 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_26 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_27 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_28 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_29 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_30 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_31 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_32 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_33 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_34 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_35 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_36 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_37 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_38 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_39 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3F => X"0000000000000000000000000000000000000000000000000000000000000000")
   port map (
      DO   => data_read(31 downto 24),
      DOP  => open, 
      ADDR => address(12 downto 2),
      CLK  => clk, 
      DI   => data_write(31 downto 24),
      DIP  => ZERO(0 downto 0),
      EN   => enable,
      SSR  => ZERO(0),
      WE   => write_byte_enable(3));

   RAMB16_S9_inst1 : RAMB16_S9
   generic map (
INIT_00 => X"a242400060406000402060b500a0c080a0b1bfb0b2b3b415b5bd00000000bd1d",
INIT_01 => X"03820083bde000b0b1b2b3b4b5bf046045021042054460800262024062208362",
INIT_02 => X"e0b000bf0000a060bfb063bdc300e042024042620342e0420200e04402404262",
INIT_03 => X"e042a40500a00040050002a00340a400e004a0440560a300a0bde0b0a000bfbd",
INIT_04 => X"6210bf000000a00080bfb0bd030000e0e0e080c0056003e38500408500006003",
INIT_05 => X"150014130000b1bfb0b2b3b4b5bdbde0b0506210bf1000000510a104bde0b050",
INIT_06 => X"040400040042404055516063435160634360235154bf53000030404210520040",
INIT_07 => X"00804470a31010000080a40600bfb0a5bdbde0b0b1b2b3b4b500bf1040425200",
INIT_08 => X"000400bde0b0bf00804470a31010000080a40600a5008040bfb082bdbde0b0bf",
INIT_09 => X"001004100080041000106242041062400343b1b2b3b4b5b6b7bebfb00302bd00",
INIT_0A => X"1000800410001004100080041000100410f71213ded6b517001e160415800410",
INIT_0B => X"6043024000000004000080041000100410008004100010041000800410001004",
INIT_0C => X"00100410111000020000008024310031044011000080041000c004004042a202",
INIT_0D => X"11000080041000c00410000460039400944000000480e0006230000000800410",
INIT_0E => X"00c0041000001110023042200000008084940094044014000080243100310440",
INIT_0F => X"2431000431000011040082021402424000000080243100310440110000800410",
INIT_10 => X"000080041000c004310004000033040044003031043200330400440030110080",
INIT_11 => X"a2001000a2400011000080041000c00410000000008024310031041110400010",
INIT_12 => X"00040080243100311110000034200004301000040040428204110080243100c0",
INIT_13 => X"6d696561363232366165696d71757900643935313e4e10000080041000041010",
INIT_14 => X"007565205b006f425b006e6470555b31386120656c6249487c00302025797571",
INIT_15 => X"62203a68746c0a627362002d2e726266697729286561612072205b006965205b",
INIT_16 => X"0000000000000000000000000000000000000000000000000000782064007820",
INIT_17 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_18 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_19 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_20 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_21 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_22 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_23 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_24 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_25 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_26 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_27 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_28 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_29 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_30 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_31 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_32 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_33 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_34 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_35 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_36 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_37 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_38 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_39 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3F => X"0000000000000000000000000000000000000000000000000000000000000000")
   port map (
      DO   => data_read(23 downto 16),
      DOP  => open, 
      ADDR => address(12 downto 2),
      CLK  => clk, 
      DI   => data_write(23 downto 16),
      DIP  => ZERO(0 downto 0),
      EN   => enable,
      SSR  => ZERO(0),
      WE   => write_byte_enable(2));

   RAMB16_S9_inst2 : RAMB16_S9
   generic map (
INIT_00 => X"18108800202828002090a00900a09890800000000000000000ff00ff0001ff50",
INIT_01 => X"e100000000001000000000000000ffff001800ff0000000020ff0000ffff0000",
INIT_02 => X"0000100000008000000000ffff000040e1ff0004e1000004e1000040e1ff0004",
INIT_03 => X"ffff38280000380028000000000010000020ff10280000100000000000100000",
INIT_04 => X"1018003000000080000000ff000000001000100028ff18382000001038380018",
INIT_05 => X"00000000908000000000000000ff000000101018000030002800ff2000000010",
INIT_06 => X"0000000000000088ffffff00ffffff00ffff00ff000000000080000081000020",
INIT_07 => X"00ff001000ff00008000000000000000ff0000000000000000100081ff00ff00",
INIT_08 => X"0100000000000000ff001000ff0000800000000000208000000000ff00000000",
INIT_09 => X"000a000000ff0000000a400100004040e1400000000000000000000001e1ff20",
INIT_0A => X"0000ff0000000a000000ff0000000a0000d000000a0a0b000000000000ff0000",
INIT_0B => X"ff00ff80000000000000ff0000000a000000ff0000000a000000ff0000000a00",
INIT_0C => X"000a000040000100000000ff0000000b0080000000ff00000080000000001010",
INIT_0D => X"000000ff000000800000010000030000ffff00000000a0010018800000ff0000",
INIT_0E => X"008000000100ff00008800ff000000ff0000000a0088000000ff0000000a0080",
INIT_0F => X"000000000a2001000000a0110011ffff000000ff0000000a0080000000ff0000",
INIT_10 => X"0000ff00000080000002000000ff0000ff011000000000000000ff01100000ff",
INIT_11 => X"00010000008002000000ff0000008000000100f800ff0000000a000040880100",
INIT_12 => X"000000ff0000000a00000100ff800000ff000000000000ff000000ff00000088",
INIT_13 => X"6e6a6662373331353964686c70747800656136320055000100ff000000000a00",
INIT_14 => X"006d6d5d64006f5d620061206c5d7539206e00726f6f534600003200307a7672",
INIT_15 => X"790000656865006f0079003e0079696f6e613a6873640064695d77006c6d5d66",
INIT_16 => X"0505050505080505050505050505050505050505050505050800292861002928",
INIT_17 => X"0002000605060505050505050505050505050505060507050805050505050505",
INIT_18 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_19 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_20 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_21 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_22 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_23 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_24 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_25 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_26 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_27 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_28 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_29 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_30 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_31 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_32 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_33 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_34 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_35 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_36 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_37 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_38 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_39 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3F => X"0000000000000000000000000000000000000000000000000000000000000000")
   port map (
      DO   => data_read(15 downto 8),
      DOP  => open, 
      ADDR => address(12 downto 2),
      CLK  => clk, 
      DI   => data_write(15 downto 8),
      DIP  => ZERO(0 downto 0),
      EN   => enable,
      SSR  => ZERO(0),
      WE   => write_byte_enable(1));

   RAMB16_S9_inst3 : RAMB16_S9
   generic map (
INIT_00 => X"2123256725252594252525c41725252525182c141c20240028d000ff0021fc00",
INIT_01 => X"0301260230082514181c2024282cfff9012b01ff000000092bff2d17fff20123",
INIT_02 => X"0810251400062508141023e8fe00080003fd0100030108000300080003fd0200",
INIT_03 => X"f8ff2b4000092513407d201d01102b000840fb21420201250718081000251418",
INIT_04 => X"2623142571000d250c1410e8018300082508250342f942252300032b25250a40",
INIT_05 => X"08c70d0a2525182c141c202428d0180810212623140125712301f52318081021",
INIT_06 => X"08203f083f100925e8a9ea1a9fc9ee1abff10ad01a2c1c004b211f1000013f25",
INIT_07 => X"00f9002110ff013f2509100a53343010c8300814181c202428252c02e310ff3f",
INIT_08 => X"0b303f3808303400f9002110ff013f250910105310252514343010c838083034",
INIT_09 => X"3f384a0000fc00013f2000500a00000000101c2024282c3034383c18b203c025",
INIT_0A => X"0000fc00013f5c0a0000fc00013f440a009010081ca81c033f00000a00fc0001",
INIT_0B => X"cf36be254b004b0a3f00fc00013f940a0000fc00013f800a0000fc00013f6c0a",
INIT_0C => X"3fb80a000000500000b200fc00013f0c0a2500b200fc00013f250a0008002180",
INIT_0D => X"00b200fc00013f250a01a62a87ff014bfffb00472dac25ac0021254b00fc0001",
INIT_0E => X"3f250a005000fd010021ff7a00b200fc00013ffc0a2500b200fc00013fe80a25",
INIT_0F => X"00013f20f82504000a3f21001002ff5e00b200fc00013fe80a2500b200fc0001",
INIT_10 => X"b200fc00013f250a010d203f00f7203fff0421012035000b203fff04210100fc",
INIT_11 => X"10a6013f10252500b200fc00013f250a0050000900fc00013fe00a0000259d00",
INIT_12 => X"ef0100fc00013fd000005000a2253f7cf7013f2e00025fe0001000fc00013f25",
INIT_13 => X"6f6b6763383430343863676b6f73770066623733004c005000fc00013f20d800",
INIT_14 => X"00702020200074202c0072626f202c0032200020616f432d0000780038007773",
INIT_15 => X"74000078206e006f00740020002e6e726769006573640061742020006c202020",
INIT_16 => X"3c3c3c3c3c603c3c3c3c3c3c3c3c3c3c3c3c3c3c3c3c3c3cc0003a6874003a68",
INIT_17 => X"030000143c6c3c3c3c3c3c3c3c3c3c3c3c3c3c3cdc3c743c8c3c3c3c3c3c3c3c",
INIT_18 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_19 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_1F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_20 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_21 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_22 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_23 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_24 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_25 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_26 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_27 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_28 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_29 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_2F => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_30 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_31 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_32 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_33 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_34 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_35 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_36 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_37 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_38 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_39 => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3A => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3B => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3C => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3D => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3E => X"0000000000000000000000000000000000000000000000000000000000000000",
INIT_3F => X"0000000000000000000000000000000000000000000000000000000000000000")
   port map (
      DO   => data_read(7 downto 0),
      DOP  => open, 
      ADDR => address(12 downto 2),
      CLK  => clk, 
      DI   => data_write(7 downto 0),
      DIP  => ZERO(0 downto 0),
      EN   => enable,
      SSR  => ZERO(0),
      WE   => write_byte_enable(0));

end; --architecture logic