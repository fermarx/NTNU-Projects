import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

def load_data(path):
    df = pd.read_excel(path, index_col=0, header=0)
    return df

def write_data(data, path):
    data.to_excel(path)
    
def write_data_csv(data, path, leave_index = False):
    data.to_csv(path, index = leave_index)

def unique_cols(df):
    a = df.to_numpy()
    return (a[0] == a).all(0)

def get_top_n(column_data, n):
    sizes = column_data.value_counts()

    df = pd.DataFrame(
        data = {'labels': sizes.index, 'sizes' : sizes.array},
        ).sort_values('sizes', ascending = False)
    
    #Selecting the top n values
    df2 = df[:n].copy()

    #Others
    new_row = pd.DataFrame(data = {
        'labels' : ['others'],
        'sizes' : [df['sizes'][n:].sum()]
    })

    #Combining the top n with the others
    df2 = pd.concat([df2, new_row], ignore_index = True)
    
    return df2

def pie_chart(column_data, top_n):
    df = get_top_n(column_data, top_n)
    
    fig1, ax1 = plt.subplots()
    ax1.pie(df['sizes'], labels=df['labels'], shadow=True, startangle=90)
    ax1.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.

    plt.show()
    
def bar_chart(column_data, top_n):
    df = get_top_n(column_data, top_n)
    df.plot.bar(0, 1)
    
def hist(column_data):
    column_data.plot.hist(column_data)
    
def scatter(column_x, column_y):
    plt.scatter(column_x, column_y)
    
def print_image(file_name):
    img = mpimg.imread(file_name)
    plt.imshow(img)
    
def join_all(data_frames):
    new_frame = data_frames[0].copy()
        
    for data_frame in data_frames[1:]:
        for column_name in data_frame.columns:
            if (not column_name == 'Customer ID') and (column_name in new_frame.columns):
                data_frame = data_frame.drop(column_name, axis = 1)
                
        new_frame = new_frame.merge(data_frame, on = 'Customer ID', sort = False, how = "inner")
        
    return new_frame

def extract_small_towns_zip_codes(population_data, small_threshold):
    small_zips = set()
    
    for i in population_data.index:
        if population_data.loc[i]['Population'] <= small_threshold:
            small_zips.add(population_data.loc[i]['Zip Code'])
            
    return small_zips

def replace_town_names_with_small_town(data, small_zips):
    j = 0
    for i in data.index:
        if data.loc[i]['Zip Code'] in small_zips:
            aux = data.loc[i]
            aux['City'] = 'Small Town'
            data.loc[i] = aux
            j += 1
            
    print(j)