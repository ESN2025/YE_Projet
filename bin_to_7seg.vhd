library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity bin_to_7seg is
port (SW : in std_logic_vector (3 downto 0);
HEX0 : out std_logic_vector (7 downto 0));
end entity bin_to_7seg;
 

architecture top_arch of bin_to_7seg is
begin
--pour les nombres positifs
	decoder: process(SW)
		begin
			case SW is
				when x"0" => HEX0 <= "11000000";		--0
				when x"1" => HEX0 <= "11111001";		--1
				when x"2" => HEX0 <= "10100100";		--2
				when x"3" => HEX0 <= "10110000";		--3
				when x"4" => HEX0 <= "10011001";		--4
				when x"5" => HEX0 <= "10010010";		--5
				when x"6" => HEX0 <= "10000010";		--6
				when x"7" => HEX0 <= "11111000";		--7
				when x"8" => HEX0 <= "10000000";		--8
				when x"9" => HEX0 <= "10010000";		--9
				when x"A" => HEX0 <= "10001000";		--A
				when x"B" => HEX0 <= "10000011";		--B
				when x"C" => HEX0 <= "10100111";		--C
				when x"D" => HEX0 <= "10100001";		--D
				when x"E" => HEX0 <= "10000110";		--E
				when x"F" => HEX0 <= "10001110";		--F
                when others => HEX0 <= "11111111";
		end case;
	end process decoder;
end architecture top_arch;