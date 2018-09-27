class TrainersController < ApplicationController

   skip_before_action :authenticate_user!

  def index

      if params[:query].present?

          params[:query] += " uk"
          @trainers = Trainer.all.near(params[:query],100)

       else

           @trainers = Trainer.where.not(latitude: nil, longitude: nil)

      end

      @markers = @trainers.map do |trainer|
      {
        lat: trainer.latitude,
        lng: trainer.longitude#,
        # infoWindow: { content: render_to_string(partial: "/flats/map_box", locals: { flat: flat }) }
      }
    end
  end
 def show
  @trainer = Trainer.find(params[:id])
  @expertise = Expertise.find(@trainer.expertise_id)
end

def new
  @user = current_user
  @trainer = Trainer.new()
  @trainer.user = @user
  @expertise = Expertise.all
end

def create

  @user = current_user
  @trainer = Trainer.new(trainer_params)

  if (@trainer.address != "")
  @trainer.address = @trainer.address + " " + @trainer.address2 + " " + @trainer.address3
  end



  @trainer.user = @user
  @expertise = Expertise.all

  if @trainer.save
    redirect_to trainer_path(@trainer)

  else

 if @trainer.hourly_rate == nil
    flash.now[:alert] = 'please enter a hourly_rate-'
  end



 if @trainer.address == ""

   if @trainer.hourly_rate == nil
    flash.now[:alert]  <<  'please enter an address-'
   else
    flash.now[:alert] = 'please enter an address-'
   end
  end


 if @trainer.address2 == ""

    if @trainer.hourly_rate == nil || @trainer.address == ""
    flash.now[:alert]  << 'please enter  a post code-'
   else
    flash.now[:alert] = 'please enter a post code-'
 end

end


 if @trainer.address3 == ""

  if  @trainer.hourly_rate == nil ||  @trainer.address == "" || @trainer.address2 == ""
    flash.now[:alert]  << 'please enter  a city-'
  else

    flash.now[:alert]  = 'please enter  a city-'
 end

end

if @trainer.photo.length == 0

 if @trainer.address3 == "" || @trainer.hourly_rate == nil ||  @trainer.address == "" || @trainer.address2 == ""

  flash.now[:alert]  << 'please upload a photo'

   else

   flash.now[:alert]  = 'please upload a photo'

  end

end



 if @trainer.first_name == ""

  if  @trainer.address3 == "" || @trainer.hourly_rate == nil ||  @trainer.address == "" || @trainer.address2 == "" || @trainer.photo.length == 0

    flash.now[:alert]  << 'please enter your first name-'


 else
   flash.now[:alert]  = 'please enter your first name-'
 end
end


   if @trainer.last_name == ""
    if @trainer.first_name == "" || @trainer.address3 == "" || @trainer.hourly_rate == nil ||  @trainer.address == "" || @trainer.address2 == "" || @trainer.photo.length == 0
    flash.now[:alert]  << 'please enter your last name-'
  else
   flash.now[:alert]  = 'please enter your last name-'
  end
 end

  render :new
  end

end

def edit


end

def update

end

def destroy
  @trainer = Trainer.find(params[:id])
  @trainer.destroy


end



private
def trainer_params


  params.require(:trainer).permit(:expertise_id, :hourly_rate, :photo, :address,:address2,:address3,:first_name,
    :last_name,:gender)

 end
end




 #














#  if params[:query].present?

#       params[:query] += "uk"
#    end
#       @trainers = Trainer.all.near(params[:query],100)

#         @markers = @trainers.map do |trainer|
#            {
#            lat: trainer.latitude,
#            lng: trainer.longitude#,

# # # infoWindow: { content: render_to_string(partial: "/flats/map_box", locals: { flat: flat }) }
#            }

#        end



#   else

#     @trainers = Trainer.all


# @trainers = Trainer.where.not(latitude: nil,longitude: nil)

# @markers = @trainers.map do |trainer|
#  {
# lat: trainer.latitude,
# lng: trainer.longitude#,
# # # infoWindow: { content: render_to_string(partial: "/flats/map_box", locals: { flat: flat }) }
# }
