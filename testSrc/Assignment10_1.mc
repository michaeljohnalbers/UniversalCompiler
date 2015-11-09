begin -- Scope level 1 (block 1)
  a := 1;
  begin -- Scope level 2 (block 2)
    a := 2;
    bb := 1;

    begin -- Scope level 3 (block 3)
      bb := 2;
      cc := 0;
    end

    dd := 1;

    begin -- Scope level 3 (block 4)
      ee := 3;

      begin -- Scope level 4 (block 5)
        ee := 2;
      end

      write(dd);
    end

    read(dd);
    dd := dd + 12;
  end

  begin -- Scope level 2 (block 6)
    ff := 2;
  end
end
