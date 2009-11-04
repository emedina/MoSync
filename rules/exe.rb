require 'rules/task.rb'
require 'rules/gcc_flags.rb'
require 'rules/loader_md.rb'

class CompileGccTask < FileTask
		
	def initialize(work, name, source, cflags)
		super(work, name)
		@SOURCE = source
		@prerequisites = [source]
		
		@DEPFILE = @work.genfile(source, ".mf")
		depFlags = " -MMD -MF #{@DEPFILE}"
		
		# only if the file is not already needed do we care about extra dependencies
		if(!needed?) then
			@prerequisites = MakeDependLoader.load(@DEPFILE, @NAME)
		end
		
		@CFLAGS = cflags + depFlags

		# save cflags to disk, for use as dependency.
		# but first read it back from disk, if existing.
		@FLAGSFILE = @work.genfile(source, ".flags")
		if(File.exists?(@FLAGSFILE)) then
			@OLDFLAGS = open(@FLAGSFILE) { |f| f.read }
		end
	end
	
	def needed?
		return true if @OLDFLAGS != @CFLAGS
		return true if !File.exists?(@DEPFILE)
		super
	end
	
	def execute
		if(@OLDFLAGS != @CFLAGS) then
			open(@FLAGSFILE, 'w') { |f| f.write(@CFLAGS) }
		end
		sh "gcc -o #{@NAME}#{@CFLAGS} -c #{@SOURCE}"
	end
end

class ExeTask < FileTask
	def initialize(work, name, objects)
		super(work, name)
		@prerequisites = @objects = objects
	end
	
	def execute
	end
end

# Compiles C/C++ code into an executable file.
# Supports GCC on linux and mingw
class ExeWork < Work
	def genfile(source, ending)
		@BUILDDIR + File.basename(source.to_s).ext(ending)
	end
	
	private

	include GccFlags

	def setup2
		define_cflags
		@CFLAGS_MAP = { ".c" => @CFLAGS,
			".cpp" => @CPPFLAGS,
			".cc" => @CPPFLAGS }

		#root task: exe file
		target = @BUILDDIR + @NAME + EXE_FILE_ENDING
		#@tasks = {target => @root}
		
		#find source files
		cfiles = collect_files(".c")
		cppfiles = collect_files(".cpp") + collect_files(".cc") 
		all_sourcefiles = cfiles + cppfiles

		source_objects = objects(all_sourcefiles)
		all_objects = source_objects + @EXTRA_OBJECTS
		@prerequisites |= ExeTask.new(self, target, all_objects)
 	end
	
	# returns an array of Tasks
	def collect_files(ending)
		files = @SOURCES.collect {|dir| Dir[dir+"/*"+ending]}
		files.flatten!
		files.reject! {|file| @IGNORED_FILES.member?(File.basename(file))}
		files += @EXTRA_SOURCEFILES.select do |file| file.endsWith(ending) end
		return files.collect do |file| FileTask.new(self, file) end
	end
	
	def makeGccTask(source, ending)
		objName = genfile(source, ending)
		ext = source.to_s.getExt
		cflags = @CFLAGS_MAP[ext]
		#puts "Ext: '#{ext}' from source '#{source}'"
		if(cflags == nil) then
			error "Bad ext: '#{ext}' from source '#{source}'"
		end
		#puts "got flags: #{cflags}"
		cflags += @SPECIFIC_CFLAGS.fetch(File.basename(source.to_s), "")
		task = CompileGccTask.new(self, objName, source, cflags)
		return task
	end
	
	# returns an array of Tasks
	def objects(sources)
		return sources.collect do |path| makeGccTask(path, ".o") end
	end
end