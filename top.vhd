LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
 
ENTITY top IS
PORT(
            key_reset          : in  std_logic ;
            clk                : in  std_logic  ;                             -- clk
            key1port_export    : in  std_logic ;                                -- export                                        
            seg_24_port_export : out std_logic_vector(47 downto 0);                    -- export 
            o_sda_export       : inout std_logic  ;               
            o_scl_export        : inout   std_logic ;                            
            cs_n : out std_logic := '1';
		    alt_adr : out std_logic := '1'

            );  -- export
END ENTITY top;


ARCHITECTURE ART OF top IS

    component system_nios is
        port (
            clk_clk                             : in    std_logic                     := 'X'; -- clk
            counter10_export                    : out   std_logic_vector(23 downto 0);        -- export
            key1port_export                     : in    std_logic                     := 'X'; -- export
            opencores_i2c_0_export_0_scl_pad_io : inout std_logic                     := 'X'; -- scl_pad_io
            opencores_i2c_0_export_0_sda_pad_io : inout std_logic                     := 'X'; -- sda_pad_io
            reset_reset_n                       : in    std_logic                     := 'X'  -- reset_n

        );
    end component system_nios;


component bin_to_7seg is
port (SW : in std_logic_vector (3 downto 0);
HEX0 : out std_logic_vector (7 downto 0));
end component bin_to_7seg;

component bin_to_7seg1 is
port (SW : in std_logic_vector (3 downto 0);
HEX0 : out std_logic_vector (7 downto 0));
end component bin_to_7seg1;

SIGNAL counter10_export :  std_logic_vector(23 downto 0); 
SIGNAL seg_export :  std_logic_vector(47 downto 0);

begin
    

    u0 : component system_nios
        port map (
            clk_clk                             => clk,                                 --                clk.clk
            counter10_export                    => counter10_export,                    --                counter10.export
            key1port_export                     => key1port_export,                     --                key1port.export
            opencores_i2c_0_export_0_scl_pad_io => o_scl_export,                        --                opencores_i2c_0_export_0.scl_pad_io
            opencores_i2c_0_export_0_sda_pad_io => o_sda_export,                        --                .sda_pad_io
            reset_reset_n                       => key_reset                            --                reset.reset_n
        );

 
--pour chaques segements positifs
    seg0 : component bin_to_7seg 
        port map (
            SW    => counter10_export(3 downto 0),
            HEX0  => seg_export(7 downto 0)
                  );
      
    seg1 : component bin_to_7seg 
        port map (
            SW    => counter10_export(7 downto 4),
            HEX0  => seg_export(15 downto 8)
                  );

    seg2 : component bin_to_7seg 
        port map (
            SW    => counter10_export(11 downto 8),
            HEX0  => seg_export(23 downto 16)
                  );

    seg3 : component bin_to_7seg 
        port map (
            SW    => counter10_export(15 downto 12),
            HEX0  => seg_export(31 downto 24)
                  );

    seg4 : component bin_to_7seg
        port map (
            SW    => counter10_export(19 downto 16),
            HEX0  => seg_export(39 downto 32)
                  ); 
						
 --pour les segements négatifs
    seg5 : component bin_to_7seg1 
        port map (
            SW    => counter10_export(23 downto 20),
            HEX0  => seg_export(47 downto 40)
                  );  
 
 
 	cs_n<='1';
	alt_adr<='1';
seg_24_port_export<=seg_export;
    
 END ARCHITECTURE ART;