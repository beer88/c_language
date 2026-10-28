import os
import glob

frontend_dir = r"c:\Users\hp\Downloads\stem_fly\frontend"

# Update index.html
index_path = os.path.join(frontend_dir, "index.html")
if os.path.exists(index_path):
    with open(index_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    target = '<li class="mb-2"><a href="pages/refund-policy.html"'
    replacement = '<li class="mb-2"><a href="pages/atl-lab-setup.html" class="text-muted text-decoration-none"><i class="bi bi-chevron-right me-1 small"></i>ATL Lab Setup</a></li>\n                            <li class="mb-2"><a href="pages/refund-policy.html"'
    
    if "pages/atl-lab-setup.html" not in content and target in content:
        content = content.replace(target, replacement, 1)
        with open(index_path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"Updated {index_path}")

# Update pages/*.html
pages_dir = os.path.join(frontend_dir, "pages")
page_files = glob.glob(os.path.join(pages_dir, "*.html"))

updated_count = 0
for p_path in page_files:
    if os.path.basename(p_path) == "atl-lab-setup.html":
        continue
    
    with open(p_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    target = '<li class="mb-2"><a href="refund-policy.html"'
    replacement = '<li class="mb-2"><a href="atl-lab-setup.html" class="text-muted text-decoration-none"><i class="bi bi-chevron-right me-1 small"></i>ATL Lab Setup</a></li>\n                            <li class="mb-2"><a href="refund-policy.html"'
    
    if "atl-lab-setup.html" not in content and target in content:
        content = content.replace(target, replacement, 1)
        with open(p_path, "w", encoding="utf-8") as f:
            f.write(content)
        updated_count += 1

print(f"Updated {updated_count} subpages under frontend/pages/")
