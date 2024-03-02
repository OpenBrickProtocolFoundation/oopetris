
local record oopetris
  api_version: string

  record InstanceUserOptions<T>
    difficulties: {T}
  end

  record InstanceUserChoiceOptions<T>
    difficulty: T | nil
  end

  record InstanceOptions 
      author: string | nil
      name: string | nil
      size: integer
      user_options: InstanceUserOptions | nil
  end

  enum Platform
    "pc"
    "android"
    "console"
  end

  record FiledSize
    width: integer
    height: integer
  end

  record Field
    size: FiledSize
  end

  record StartParameters 
    target_fps: integer
    platform: Platform
    field: Field
  end

  record Instance<T>
    on_start:function(function(start_parameters: StartParameters, options: InstanceUserChoiceOptions<T>))

  end

  register_instance: function<T>(options: InstanceOptions<T>): Instance<T> | nil, string | nil
end


return oopetris
