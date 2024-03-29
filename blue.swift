import MetalKit

struct Render_Rect 
{
	var x : Float = 0.0
	var y : Float = 0.0

	var w : Float = 0.0
	var h : Float = 0.0

	var r : Float = 0.0
	var g : Float = 0.0
	var b : Float = 0.0
	var a : Float = 1.0

	var u : Float = 0.0
	var v : Float = 0.0

	var i : UInt16  = 0
}

struct Vertex 
{
	var position : simd_float4 = SIMD4<Float>(0,0,0,0)
	var colour 	 : simd_float4 = SIMD4<Float>(0,0,0,0)
	//var uv	 : simd_float2
	//var texture  : UInt16
}

struct Input
{
	// Using SIMD<> to make sure the memory is aligned as expected
	var mouse_position 	: SIMD2<Float>	= SIMD2<Float>(0,0)
	var mouse_down 		: Bool 			= false
	var window_width 	: UInt16  		= 0
	var window_height 	: UInt16		= 0
}	

struct Render_Commands
{
	var command_count 	: Int = 0
	var commands 		= [Render_Rect](repeating: Render_Rect(), count:Max_Render_Command_Count)
}

struct Vertices
{
	var vertex_count 	: Int = 0
	var vertices	 	= [Vertex](repeating: Vertex(), count:Max_Vertex_Count)
}

let Max_Render_Command_Count = 4096
let Max_Vertex_Count = Max_Render_Command_Count * 6

var render_commands = Render_Commands()
var vertices = Vertices()
var input = Input()

var running = false
var t = 0.0

public typealias update_callback_t = @convention(c) (Float, UnsafeMutableRawPointer) -> ()
public typealias render_callback_t = @convention(c) ()->()

var app:NSApplication!

func push_vertex (_ v:inout Vertices, x:Float, y:Float, r:Float, g:Float, b:Float, a:Float)
{
	assert (v.vertex_count < Max_Vertex_Count-1)

	v.vertices[v.vertex_count].position[0] = x
	v.vertices[v.vertex_count].position[1] = y
	v.vertices[v.vertex_count].position[2] = 0
	v.vertices[v.vertex_count].position[3] = 1

	v.vertices[v.vertex_count].colour[0] = r
	v.vertices[v.vertex_count].colour[1] = g
	v.vertices[v.vertex_count].colour[2] = b
	v.vertices[v.vertex_count].colour[3] = a

	v.vertex_count += 1
}

@_cdecl ("push_rect") 
public func push_rect (x:Float, y:Float, w:Float, h:Float, r:Float, g:Float, b:Float, a:Float, i:UInt16)
{
	assert(render_commands.command_count<Max_Render_Command_Count-1)

	render_commands.commands[render_commands.command_count].x = x
	render_commands.commands[render_commands.command_count].y = y

	render_commands.commands[render_commands.command_count].w = w
	render_commands.commands[render_commands.command_count].h = h

	render_commands.commands[render_commands.command_count].r = r
	render_commands.commands[render_commands.command_count].g = g
	render_commands.commands[render_commands.command_count].b = b
	render_commands.commands[render_commands.command_count].a = a 

	render_commands.commands[render_commands.command_count].u = 0
	render_commands.commands[render_commands.command_count].v = 0

	render_commands.commands[render_commands.command_count].i = i

	render_commands.command_count += 1
}


private class App_Delegate: NSObject, NSApplicationDelegate 
{
    func applicationDidFinishLaunching(_ notification: Notification) {}

    func applicationWillTerminate(_ notification: Notification) 
    {
        running = false
    }
}

private class Window_Delegate : NSObject, NSWindowDelegate
{
    func windowWillClose(_ notification: Notification) { running = false }
}



@_cdecl ("start_app")
public func start_app ()
{
	let app_delegate = App_Delegate()
	app = NSApplication.shared
	app.delegate = app_delegate
	app.setActivationPolicy(.regular)
	app.finishLaunching()
	app.activate(ignoringOtherApps:true)

	input.window_width = 512
	input.window_height = 384
}

var shader:String = ""

@_cdecl ("open_window")
public func open_window (
	update_function:update_callback_t, 
	rc:@escaping render_callback_t,
	shader_path:UnsafePointer<CChar>)
{
	shader = String(utf8String:shader_path)!
	let frame = NSRect(x:0, y:0, width:Int(input.window_width), height:Int(input.window_height))
	let delegate = Window_Delegate()
	let window = NSWindow(
		contentRect: frame, 
		styleMask: [.titled, .closable, .miniaturizable, .resizable], backing: .buffered, 
		defer: false)

	let metal_view = Window_View(frame:frame, device:MTLCreateSystemDefaultDevice())
	metal_view.colorPixelFormat = .bgra8Unorm
	metal_view.depthStencilPixelFormat = .depth32Float
	metal_view.preferredFramesPerSecond = 60
	metal_view.isPaused = true
	metal_view.enableSetNeedsDisplay = true
	
	let renderer = Renderer(device: metal_view.device!)
	renderer.render_function = rc
	metal_view.delegate = renderer
	
	window.delegate = delegate
	window.title = "blue"
	window.contentView = metal_view
	window.center()
	window.orderFrontRegardless()
	window.contentView?.updateTrackingAreas()

	running = true

	var lastTime = Date()
	var frame_counter : Double = 0

	input.window_width = 1024
	input.window_height = 768

	//input.window_width  = UInt16(window.contentView?.bounds.width ?? 1024)
	//input.window_height = UInt16(window.contentView?.bounds.height ?? 768)
	
	// TODO: Move the message pump somewhere independent of the windows??
	while (running)
	{
	    var event:NSEvent?
	    repeat {
	        event = app.nextEvent(matching: .any, until: nil, inMode: .default, dequeue: true)
	        
	        if event != nil { app.sendEvent(event!) }

	    } while(event != nil)

	    let delta = lastTime.timeIntervalSinceNow
	    t -= delta
	    frame_counter -= delta

	    // Only draw (roughly) every 60th of a second.
	    if (frame_counter > 1.0/60.0) {
	    	metal_view.setNeedsDisplay(metal_view.bounds)
	    	frame_counter = 0.0
	    }

	    lastTime = Date()

	    update_function (Float(t), &input)
	}
}


private class Window_View : MTKView
{
    var tracking_area:NSTrackingArea? = nil

    override func updateTrackingAreas()
    {
        // print(#function)
        if tracking_area != nil { removeTrackingArea(tracking_area!) }
        tracking_area = NSTrackingArea(rect: self.bounds, options: [.activeAlways, .mouseMoved] , owner: self, userInfo: nil)
        addTrackingArea(tracking_area!)
    }
    
    override func mouseExited(with event: NSEvent)
    {
        // print(#function)
        super.mouseExited(with: event)
    }
    
    override func mouseEntered(with event: NSEvent)
    {
        // print(#function)
        super.mouseEntered(with: event)
    }
    
    override func mouseMoved(with event: NSEvent)
    {
        // print(#function)
        super.mouseMoved(with: event)
        mouse_event(event)
    }
    
    override func mouseDragged(with event: NSEvent)
    {
        // print(#function)
        super.mouseDragged(with: event)
        mouse_event(event)
    }
    
    override func mouseDown(with event: NSEvent)
    {
        // print(#function)
        super.mouseDown(with: event)
        input.mouse_down = true
        mouse_event(event)
        
    }
    
    override func mouseUp(with event: NSEvent)
    {
        // print(#function)
        super.mouseUp(with: event)
        input.mouse_down = false
        mouse_event(event)
        
    }

    func mouse_event (_ event:NSEvent)
    {
        let window_size = event.window!.contentView!.bounds.size
        let point = CGPoint(x: event.locationInWindow.x / window_size.width, y: 1 - event.locationInWindow.y / window_size.height)

        input.mouse_position.x = Float(point.x)
        input.mouse_position.y = Float(point.y)
    }
}

private class Renderer: NSObject, MTKViewDelegate
{
	var render_function:(()->())!

    var device: MTLDevice
    var commandQueue: MTLCommandQueue
    var library:MTLLibrary!
    var vertex_shader:MTLFunction!
    var fragment_shader:MTLFunction!
    var pipelineState: MTLRenderPipelineState!
    var vertex_buffer: MTLBuffer!

    init (device: MTLDevice)
    {
        self.device = device
        self.commandQueue = device.makeCommandQueue()!
        super.init()

        if let l = try? self.device.makeLibrary(filepath: shader) {
        // if let l = try? self.device.makeLibrary(filepath: "shaders.metallib") {
        	self.library = l
        	self.init_callback()
        }
    }

    // TODO: Try re-using this when the metallib changes.
    func init_callback ()
    {   
        vertex_shader   = library.makeFunction(name:"vertex_func")
        fragment_shader = library.makeFunction(name:"fragment_func")

        if vertex_shader == nil || fragment_shader == nil { fatalError("Couldn't load all the required shaders.") }

        let pipeline_state_descriptor = MTLRenderPipelineDescriptor()
        pipeline_state_descriptor.vertexFunction   = vertex_shader
        pipeline_state_descriptor.fragmentFunction = fragment_shader
        pipeline_state_descriptor.colorAttachments[0].pixelFormat = .bgra8Unorm

        pipelineState = try! device.makeRenderPipelineState(descriptor: pipeline_state_descriptor)

        if pipelineState == nil { fatalError("Couldn't create the pipeline state.") }
        else { print ("Created the pipeline state OK") }
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize)
    {
        // print ("\(#function) \(size)")
        input.window_width  = UInt16(size.width)
        input.window_height = UInt16(size.height)
        (view as! Window_View).updateTrackingAreas()
    }

    func draw(in view: MTKView)
    {
    	if (pipelineState == nil) { print("invalid pipelineState"); return }

    	// Varying background so we know we're doing something
        struct Color { var red, green, blue, alpha: Double }
        let color = Color(red: sin(t), green: 0.75, blue: 1.0, alpha: 1.0)
        view.clearColor = MTLClearColorMake(color.red, color.green, color.blue, color.alpha)
        

        let render_pass_descriptor:MTLRenderPassDescriptor = view.currentRenderPassDescriptor!
        let command_buffer = commandQueue.makeCommandBuffer()
        let render_encoder:MTLRenderCommandEncoder = (command_buffer?.makeRenderCommandEncoder(descriptor: render_pass_descriptor))!

        render_encoder.setRenderPipelineState(pipelineState)


        // Draw the user submitted stuff

        render_commands.command_count = 0
        vertices.vertex_count = 0

        render_function?()

        for i in 0..<render_commands.command_count {

        	let c : Render_Rect = render_commands.commands[i]

        	push_vertex(&vertices, x:c.x,     y:c.y,     r:c.r, g:c.g, b:c.b, a:c.a)
        	push_vertex(&vertices, x:c.x+c.w, y:c.y,     r:c.r, g:c.g, b:c.b, a:c.a)
        	push_vertex(&vertices, x:c.x,     y:c.y+c.h, r:c.r, g:c.g, b:c.b, a:c.a)
        	push_vertex(&vertices, x:c.x,     y:c.y+c.h, r:c.r, g:c.g, b:c.b, a:c.a)
        	push_vertex(&vertices, x:c.x+c.w, y:c.y,     r:c.r, g:c.g, b:c.b, a:c.a)
        	push_vertex(&vertices, x:c.x+c.w, y:c.y+c.h, r:c.r, g:c.g, b:c.b, a:c.a)
        }

        let data_size = vertices.vertex_count * MemoryLayout.size(ofValue: vertices.vertices[0])
        	vertex_buffer = device.makeBuffer(bytes: vertices.vertices, length: data_size, options: [])
        	render_encoder.setVertexBuffer(vertex_buffer, offset: 0, index: 0)
        	render_encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertices.vertex_count, instanceCount: 1)

        // /user submitted commands



        render_encoder.endEncoding()

        command_buffer!.present(view.currentDrawable!)
        command_buffer!.commit()
    }
}

