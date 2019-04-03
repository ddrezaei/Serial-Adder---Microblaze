----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    09:10:22 10/02/2017 
-- Design Name: 
-- Module Name:    contUnit - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.std_logic_unsigned.all;


entity contUnit is
  port(
		 clk    : in std_logic;
		 resL    : in std_logic;
		 start  : in std_logic;
		 ready  : out std_logic;
		 shen  : out std_logic;
		 init  : out std_logic
		);
end contUnit;

architecture contUnit_imp of contUnit is

  constant waiting1    : std_logic_vector(1 downto 0)   := "00";
  constant waiting0    : std_logic_vector(1 downto 0)   := "01";
  constant adding1_8   : std_logic_vector(1 downto 0)   := "10";
  	
  signal current_state, next_state	  : std_logic_vector(1 downto 0);
  signal r                            : std_logic_vector(3 downto 0);
  signal rTemp                        : std_logic_vector(3 downto 0);
  signal s	                          : std_logic_vector(2 downto 0);
  signal cntComplete, cen             : std_logic;
  
begin

  ALWAYS_1 : process( clk, resL ) is
  begin
    		if (resL = '0') then
				current_state <= waiting1;
			elsif ( clk = '1' and clk'event ) then
				current_state <= next_state;
			end if;	
  end process ALWAYS_1;

  ALWAYS_2 : process( clk, current_state, start, cntComplete ) is
  begin
		case current_state is
				when waiting1 =>                 -- wait 1 for start
					if (start='1') then
						next_state <= waiting0;
					else
						next_state <= waiting1;
					end if;	
				when waiting0 =>                 -- wait 0 for start
					if (start='1') then
						next_state <= waiting0;
					else
						next_state <= adding1_8;
					end if;	
				when adding1_8 =>
					if (cntComplete='1') then
						next_state <= waiting1;
					else
						next_state <= adding1_8;
					end if;	
				when others =>
				   null;
		end case;
  end process ALWAYS_2;

  ALWAYS_3 : process( current_state ) is
  begin
		case current_state is
				when waiting1 =>
					ready <= '1';
					shen <= '0';
					init <= '0';
					cen <= '0';
				when waiting0 =>
					ready <= '0';
					shen <= '0';
					init <= '1';
					cen <= '0';
				when adding1_8 =>
					ready <= '0';
					shen <= '1';
					init <= '0';
					cen <= '1';
				when others =>
				   null;
		end case;
  end process ALWAYS_3;

  ALWAYS_4 : process( clk, resL ) is
  begin
    		if (resL = '0') then
				r <= "0000";
			elsif ( clk = '1' and clk'event ) then
			  if (cen = '1') then
				 r <= r + 1;
			  end if;
			end if;	
  end process ALWAYS_4;

  rTemp <= r + 1;
  s <= rTemp(2 downto 0);
  cntComplete <= rTemp(3);

end contUnit_imp;

