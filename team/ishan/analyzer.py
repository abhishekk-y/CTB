import ctypes
import json
import os

class LogAnalyzer:
    def __init__(self):
        lib_path = os.path.join(os.path.dirname(__file__), "..", "liblogsentinel.so")
        if not os.path.exists(lib_path):
            raise FileNotFoundError(f"C++ Engine Library not found at {lib_path}. Please compile first.")
        
        self.lib = ctypes.CDLL(lib_path)
        
        # parse_logs returns a newly allocated char*
        self.lib.parse_logs.argtypes = [ctypes.c_char_p]
        self.lib.parse_logs.restype = ctypes.POINTER(ctypes.c_char)
        
        # free_memory takes a char* and frees it
        self.lib.free_memory.argtypes = [ctypes.POINTER(ctypes.c_char)]
        self.lib.free_memory.restype = None

    def analyze(self, filepath):
        """Pass log file to C++ engine, parse returned JSON."""
        result_ptr = self.lib.parse_logs(filepath.encode('utf-8'))
        
        if not result_ptr:
            return {"error": "C++ engine returned null pointer"}
        
        # Read the C string into Python
        json_bytes = ctypes.string_at(result_ptr)
        json_str = json_bytes.decode('utf-8')
        
        # Free the C++ allocated memory
        self.lib.free_memory(result_ptr)
        
        try:
            data = json.loads(json_str)
            if isinstance(data, list):
                return data
            return data
        except json.JSONDecodeError as e:
            return {"error": f"JSON decode error: {e}, raw: {json_str[:200]}"}
