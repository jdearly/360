with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Strings.Unbounded; use Ada.Strings.Unbounded;
with Ada.Strings.Unbounded.Text_IO; use Ada.Strings.Unbounded.Text_IO;
with Ada.Characters.Handling; use Ada.Characters.Handling;


procedure editdis is

    type CostArray is array(1..4) of Natural; -- Array type to hold cost values.
                                              -- Easier to pass around than 4
                                              -- additional parameters
	type states is (outside_a_word, inside_a_word);

   -- Simple utility function to take the minimum of three natural numbers
   function Min(one : Natural; two : Natural) return Natural is
   begin
      if (one < two) then
         return one;
      else
         return two;
      end if;
   end Min;

   function Min(one : Natural; two : Natural; three : Natural) return Natural is
   begin
        return Min(Min(one, two), three);
   end Min;

   -- Function to return the edit distance between `search_word` and `input_word`
   function EditDistance(search_word : in String; input_word : in String; cost_values: CostArray) return Natural is
      matrix : array(1 .. search_word'Length+1, 1 .. input_word'Length+1) of Natural; -- 2d matrix of the edit distances
      inc : Natural;  -- used to determine how much to increment the number by
      A,B : Natural;  -- used to hold the indices for MinDist
      c_cost: Natural := cost_values(1);
      d_cost: Natural := cost_values(2);
      i_cost: Natural := cost_values(3);
      s_cost: Natural := cost_values(4);
   begin
      -- fill in the zeroth row with incrementing numbers
      for I in 1 .. search_word'Length+1 loop
         matrix(I, 1) := I - 1;
      end loop;

      -- fill in the zeroth column with incrementing numbers
      for J in 1 .. input_word'Length+1 loop
         matrix(1, J) := J - 1;
      end loop;

      -- fill in the rest of the matrix based on previous entries
      for I in 1 .. search_word'Length loop
         A := I + 1;
         for J in 1 .. input_word'Length loop
            B := J + 1;
            if To_Lower(search_word(I)) = To_Lower(input_word(J)) then
               inc := c_cost;
            else
               inc := s_cost;
            end if;
               matrix(A, B) := Min(matrix(A - 1, B) + d_cost,
                                       matrix(A, B - 1) + i_cost,
                                       matrix(A - 1, B - 1) + inc);
         end loop;
      end loop;
      -- return the last element in the table
      return matrix(search_word'Length + 1, input_word'Length + 1);
   end EditDistance;

   -- Procedure to get the next word from the input stream or place
   -- whitespace and punctuation is ignored
   -- State machine for input
   -- (0) -- (1) on a character that is not whitespace or punctuation (alpha)
   -- (0) -- (2) on a character that is whitespace or punctuation (beta)
   -- (1) -- (1) on alpha
   -- (1) -- (3) on beta
   -- (2) -- (2) on alpha
   -- (3) is the accept state, the machine rolls back at that point
   procedure process(search_word : in String; maximum_distance : in Natural; cost_values: CostArray) is
      buffer   : String(1 .. 256); -- allow for 256 characters to be read if needed
      buffLen  : Natural;         -- keep an offset to the last character
      index    : Natural;         -- index for the buffer
      c        : Character;
      dist     : Natural;
      EOL      : Boolean;
	  state    : states := outside_a_word;         -- which state are we in

   procedure check_dist is
   begin
    if state = inside_a_word then       -- if the state is 1 we are ready to check the edit distance
        dist := EditDistance(search_word, buffer(1 .. buffLen), cost_values);
        if (dist <= maximum_distance) then
           Put("(" & buffer(1..buffLen) & ")");
        else
           Put(buffer(1..buffLen));
        end if;
		if not is_letter(c) then
        Put(c);  -- put the special or whitespace character after the word has been processed
	end if;
        -- reset the variables
        state := outside_a_word;          -- go back to state 1
        index := 1;
        buffLen := 0;
     end if;
   end check_dist;

   begin

      --state := 0;                 -- start in state 0
      index := 1;                 -- set it to point to the first element
      buffLen := 0;               -- the current length is zero
	 -- c := ' ';
	  
      -- cycle through all of the characters
      while not end_of_file loop
		 look_ahead(c, EOL);
		 if EOL then
			case state is
			    when inside_a_word =>
					check_dist;
					new_line;
					skip_line;
					state := outside_a_word;
				when others =>
					new_line;
					skip_line;
					--state := outside_a_word;
			end case;
		  else
			  get(c);
		 end if;
		-- get(c);
		 case state is
			when outside_a_word =>
				if not is_letter(c) then
					put(c);
				else
					buffer(index) := c;
					buffLen := buffLen + 1;
					index := index + 1;
					state := inside_a_word;
				end if;
			 when inside_a_word =>
				 if is_letter(c) then
					buffer(index) := c;
					buffLen := buffLen + 1;
					index := index + 1;
				 else
					check_dist;
					state := outside_a_word;
				 end if;
		 end case; 
         -- check if the next character is the EOL
        -- if EOL then
         --   case state is
           --    when 1 =>
             --     state := 3;       -- state three is the accept state
             --  when others =>
             --     New_Line;
             --     Skip_Line;
           -- end case;
           -- check_dist;
        -- else
             -- check if it is whitespace or punctuation
            -- if Is_Special(c) then -- skip the punctuation
            --    case state is
            --       when 1 =>
            --          state := 3;
            --       when others =>
            --          Put(c);
            --    end case;
            -- else     -- else its a valid character to use to determine if it's within edit distance
            --    buffer(index) := c;        -- place the character into the buffer
            --    buffLen := buffLen + 1;    -- increment the buffer length
            --    index := index + 1;        -- increment the index
            --    state := 1;
            -- end if;

           -- Get(c);      -- consume the character
          --end if;
      end loop;
	  check_dist;
   end process;

   search_word  : Unbounded_String;  -- used to hold the search word from input
   maximum_distance     : Natural;           -- used to hold the maximum allowable edit distance
   cost: Natural; -- for getting next cost value from input
   cost_values: CostArray; -- holds the value of each cost
begin
   Get_Line(search_word); -- get the search word from input
   Put_Line(search_word); -- print the search word
   Get(maximum_distance);  -- get the maximum distance value from input
   for i in cost_values'range loop
     Get(cost);
     cost_values(i) := cost;
   end loop;

   Put(maximum_distance, 1);
   New_Line;
   for i in cost_values'range loop
     Put(cost_values(i), 1);
     Put(" ");
   end loop;
   process(to_string(search_word), maximum_distance, cost_values);
end editdis;
