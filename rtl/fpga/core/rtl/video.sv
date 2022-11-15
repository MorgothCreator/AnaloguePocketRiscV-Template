// We pixel double our video output for scaling for the Pocket. Since it's a
// non-integer scale, we use half pixels on either side for pixels 0
module video # (
	parameter PLATFORM = "XILINX",
	parameter BUS_ADDR_DATA_LEN = 8,
	parameter PORT_WIDTH = 32,
	
	parameter CORE_CLK_FREQ = 74.25,
	parameter PIXEL_CLOCK = 25.00,
	
	parameter BASE_ADDR = 'h00,
	parameter PIXEL = 'h00,
	parameter SET_X_H = 'h01,
	parameter SET_X_L = 'h02,
	parameter SET_Y_H = 'h03,
	parameter SET_Y_L = 'h04,
	parameter SET_WIN_START_X_H = 'h05,
	parameter SET_WIN_START_X_L = 'h06,
	parameter SET_WIN_END_X_H = 'h07,
	parameter SET_WIN_END_X_L = 'h08,
	parameter SET_WIN_START_Y_H = 'h09,
	parameter SET_WIN_START_Y_L = 'h0A,
	parameter SET_WIN_END_Y_H = 'h0B,
	parameter SET_WIN_END_Y_L = 'h0C,
	parameter SET_ACTION = 'h0D,
	parameter STATUS = 'h0E
)(
	input rst_i,
	input clk_core_i,
	
	input [BUS_ADDR_DATA_LEN-1:0]addr_i,
	input wrb_i,
	input [PORT_WIDTH - 1:0]bus_i,
	input rdb_i,
	output reg [PORT_WIDTH - 1:0]bus_o,
	
	input wire clk_pixel_i,
	output reg v_sync_o,
	output reg h_sync_o,
	output reg video_en_o,
	output wire [7:0]video_o
);
localparam BUS_LEN_SHIFT = PORT_WIDTH > 16 ? 2 : (PORT_WIDTH > 8 ? 1 : 0);

  reg [7:0]ssd_vid;
  
  reg [10:0] h_count;

  reg [9:0] v_count;

  localparam [9:0] h_front_porch = 40;
  localparam [9:0] h_sync_length = 80;
  localparam [9:0] h_back_porch = 120 + h_sync_length; // 200

  localparam [9:0] v_front_porch = 3;
  localparam [9:0] v_sync_length = 10;
  localparam [9:0] v_back_porch = 15 + v_sync_length; // 25

  localparam [9:0] h_disabled = h_front_porch + h_back_porch; // 240
  localparam [9:0] v_disabled = v_front_porch + v_back_porch; // 28

  localparam [10:0] h_total = 1040;
  localparam [9:0] v_total = 748;

  localparam [9:0] h_active = 800;
  localparam [9:0] v_active = 720;

  localparam [9:0] h_active_oled = 768;
  localparam [9:0] v_active_oled = 384;

  localparam [9:0] h_spacing = 0;//(h_active - h_active_oled) / 2;
  localparam [9:0] v_spacing = 0;//(v_active - v_active_oled) / 2;
  
  localparam screen_divider = 2;

  wire [9:0] current_h;
  wire [9:0] current_v;

  assign current_h = h_count - h_disabled - h_spacing;
  assign current_v = v_count - v_disabled - v_spacing;

  reg [7:0]vram[(h_active / screen_divider) * (v_active / screen_divider)];

  assign video_o =
         h_count > h_disabled + h_spacing &&
         v_count > v_disabled + v_spacing &&
         current_h < h_active - h_spacing &&
         current_v < v_active - v_spacing
         ? ssd_vid : 8'h0;

  always @ (posedge clk_pixel_i)
  begin
	if(rst_i) begin
		h_count <= 11'h0;
		v_count <= 10'h0;
	end else begin
    v_sync_o <= 0;
    h_sync_o <= 0;
    video_en_o <= 0;

    if (h_count == 0 && v_count == v_front_porch)
    begin
      v_sync_o <= 1;
    end
    else if (h_count == h_front_porch)
    begin
      h_sync_o <= 1;
    end

    if (h_count >= h_disabled && v_count >= v_disabled)
    begin
      video_en_o <= 1;
    end

    h_count <= h_count + 1;

    if (h_count == h_total - 1)
    begin
      h_count <= 0;

      v_count <= v_count + 1;

      if (v_count == v_total - 1)
      begin
        v_count <= 0;
      end
    end
	 ssd_vid <= vram[(current_h / screen_divider) + ((current_v / screen_divider) * (h_active / screen_divider))];
	end
  end
  
  reg exec_com_p;
  reg exec_com_n;
  reg ram_write_pix;
  reg[7:0]color;
  reg[7:0]color_int;
  reg[9:0]x_count;
  reg[9:0]y_count;
  reg[9:0]x_count_int;
  reg[9:0]y_count_int;
  reg[9:0]x_start;
  reg[9:0]x_end;
  reg[9:0]y_start;
  reg[9:0]y_end;
  wire busy = ram_write_pix;
  reg[1:0]comand;
  
  localparam COMAND_IDLE				= 2'h0;
  localparam COMAND_DRAW_PIXEL		= 2'h1;
  localparam COMAND_DRAW_PIXELS		= 2'h2;
  localparam COMAND_CLEAR				= 2'h3;
  
  localparam STATUS_BUSY_BIT			= 0;
  
localparam SET_X_H_INT = BASE_ADDR + SET_X_H;
localparam SET_X_L_INT = BASE_ADDR + SET_X_L;
localparam SET_Y_H_INT = BASE_ADDR + SET_Y_H;
localparam SET_Y_L_INT = BASE_ADDR + SET_Y_L;
localparam PIXEL_INT = BASE_ADDR + PIXEL;
localparam SET_WIN_START_X_H_INT = BASE_ADDR + SET_WIN_START_X_H;
localparam SET_WIN_START_X_L_INT = BASE_ADDR + SET_WIN_START_X_L;
localparam SET_WIN_END_X_H_INT = BASE_ADDR + SET_WIN_END_X_H;
localparam SET_WIN_END_X_L_INT = BASE_ADDR + SET_WIN_END_X_L;
localparam SET_WIN_START_Y_H_INT = BASE_ADDR + SET_WIN_START_Y_H;
localparam SET_WIN_START_Y_L_INT = BASE_ADDR + SET_WIN_START_Y_L;
localparam SET_WIN_END_Y_H_INT = BASE_ADDR + SET_WIN_END_Y_H;
localparam SET_WIN_END_Y_L_INT = BASE_ADDR + SET_WIN_END_Y_L;
localparam SET_ACTION_INT = BASE_ADDR + SET_ACTION;
localparam STATUS_INT = BASE_ADDR + STATUS;

  
  always @ * begin
		bus_o = {PORT_WIDTH{1'b0}};
		if(rdb_i) begin
			case(addr_i)
				STATUS_INT: bus_o = {{PORT_WIDTH - 1{1'b0}}, busy};
			endcase
		end
  end
   
  always @ (posedge clk_core_i) begin
	if(rst_i) begin
		exec_com_p <= 1'b0;
		x_count <= 10'h0;
		y_count <= 10'h0;
		comand <= COMAND_DRAW_PIXEL;
	end else begin
		if(wrb_i/* && ~busy*/) begin // Make sure the unit is not busy
			case(addr_i)
				SET_X_H_INT: x_count[9:8] <= bus_i[1:0];
				SET_X_L_INT: x_count[7:0] <= bus_i[7:0];
				SET_Y_H_INT: y_count[9:8] <= bus_i[1:0];
				SET_Y_L_INT: y_count[7:0] <= bus_i[7:0];
				PIXEL_INT: begin
					if(exec_com_n == exec_com_p) begin
						exec_com_p <= ~exec_com_p;
					end
					case(comand)
						COMAND_DRAW_PIXEL: begin
							x_count <= x_count + 10'h01;
							if(x_count == (h_active / screen_divider)) begin
								x_count <= 10'h00;
								y_count <= y_count + 10'h01;
							end
							if(y_count == (v_active / screen_divider)) begin
								x_count <= 10'h00;
								y_count <= 10'h00;
							end
							color <= bus_i[7:0];
						end
						COMAND_DRAW_PIXELS: begin
							color <= bus_i[7:0];
						end
						COMAND_CLEAR: begin
							x_start <= 10'd0;
							x_end <= 10'd399;
							y_start <= 10'd0;
							y_end <= 10'd359;
							color <= bus_i[7:0];
						end
					endcase
				end
				SET_WIN_START_X_H_INT: begin
					x_start[9:8] <= bus_i[1:0];
					x_end[9:8] <= bus_i[1:0];
				end
				SET_WIN_START_X_L_INT: begin
					x_start[7:0] <= bus_i[7:0];
					x_end[7:0] <= bus_i[7:0];
				end
				SET_WIN_END_X_H_INT: x_end[9:8] <= bus_i[1:0];
				SET_WIN_END_X_L_INT: x_end[7:0] <= bus_i[7:0];
				SET_WIN_START_Y_H_INT: begin
					y_start[9:8] <= bus_i[1:0];
					y_end[9:8] <= bus_i[1:0];
				end
				SET_WIN_START_Y_L_INT: begin
					y_start[7:0] <= bus_i[7:0];
					y_end[7:0] <= bus_i[7:0];
				end
				SET_WIN_END_Y_H_INT: y_end[9:8] <= bus_i[1:0];
				SET_WIN_END_Y_L_INT: y_end[7:0] <= bus_i[7:0];
				SET_ACTION_INT: comand <= bus_i[7:0];
			endcase
		end
	end
  end
/* Use the fastest clock available */
  always @ (posedge CORE_CLK_FREQ < PIXEL_CLOCK ? clk_pixel_i : clk_core_i) begin
	if(rst_i) begin
		exec_com_n <= 1'b0;
		ram_write_pix <= 1'b0;
		x_count_int <= 10'h0;
		y_count_int <= 10'h0;
	end else begin
		ram_write_pix <= 1'b0;
		if(ram_write_pix) begin
			case(comand)
				COMAND_DRAW_PIXEL: begin
					vram[x_count_int + (y_count_int * (h_active / screen_divider))] <= color_int;
				end
				COMAND_DRAW_PIXELS,
				COMAND_CLEAR: begin
					x_count_int <= x_count_int + 10'h01;
					ram_write_pix <= 1'b1;
					if(x_count_int == x_end) begin
						x_count_int <= x_start;
						y_count_int <= y_count_int + 10'h01;
					end
					if(x_count_int == x_end && y_count_int == y_end) begin
						x_count_int <= x_start;
						y_count_int <= y_start;
						ram_write_pix <= 1'b0;
					end else
						vram[x_count_int + (y_count_int * (h_active / screen_divider))] <= color_int;
				end
			endcase
		end
		if(exec_com_n != exec_com_p) begin
			exec_com_n <= ~exec_com_n;
			color_int <= color;
			ram_write_pix <= 1'b1;
			case(comand)
				COMAND_DRAW_PIXEL: begin
					x_count_int <= x_count;
					y_count_int <= y_count;
				end
				COMAND_DRAW_PIXELS,
				COMAND_CLEAR: begin
					x_count_int <= x_start;
					y_count_int <= y_start;
				end
			endcase
		end
	end
  end
  
endmodule
